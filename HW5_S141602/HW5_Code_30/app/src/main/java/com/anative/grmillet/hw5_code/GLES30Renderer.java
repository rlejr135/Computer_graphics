package com.anative.grmillet.hw5_code;

import android.content.Context;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;

import javax.microedition.khronos.opengles.GL10;

public class GLES30Renderer implements GLSurfaceView.Renderer {

    private Context mContext;


    Camera mCamera;
    private Mario mMario;
    private tank mTank;
    private Bus mBus;
    private building mBuilding;

    private float rotangle = 0.0f;

    public float ratio = 1.0f;
    public int headLightFlag = 1;
    public int lampLightFlag = 1;
    public int pointLightFlag = 1;
    public int cowLightFlag = 1;
    public int textureFlag = 1;

    public float[] mMVPMatrix = new float[16];
    public float[] mProjectionMatrix = new float[16];
    public float[] mModelViewMatrix = new float[16];
    public float[] mModelMatrix = new float[16];
    public float[] mViewMatrix = new float[16];
    public float[] mModelViewInvTrans = new float[16];

    final static int TEXTURE_ID_MARIO = 0;
    final static int TEXTURE_ID_TANK = 1;
    final static int TEXTURE_ID_BUS = 2;
    final static int TEXTURE_ID_BUILDING = 3;

    private ShadingProgram mShadingProgram;

    public GLES30Renderer(Context context) {
        mContext = context;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, javax.microedition.khronos.egl.EGLConfig config) {
        GLES30.glClearColor(0.0f, 0.0f, 0.8f, 1.0f);

        GLES30.glEnable(GLES30.GL_DEPTH_TEST);

        // 초기 뷰 매트릭스를 설정.
        mCamera = new Camera();

        //vertex 정보를 할당할 때 사용할 변수.
        int nBytesPerVertex = 8 * 4;        // 3 for vertex, 3 for normal, 2 for texcoord, 4 is sizeof(float)
        int nBytesPerTriangles = nBytesPerVertex * 3;

        /*
            우리가 만든 ShadingProgram을 실제로 생성하는 부분
         */
        mShadingProgram = new ShadingProgram(
            AssetReader.readFromFile("vertexshader.vert" , mContext),
            AssetReader.readFromFile("fragmentshader.frag" , mContext));
        mShadingProgram.prepare();
        mShadingProgram.initLightsAndMaterial();
        mShadingProgram.initFlags();
        mShadingProgram.set_up_scene_lights(mViewMatrix);

        /*
                우리가 만든 Object들을 로드.
         */
        mMario = new Mario();
        mMario.addGeometry(AssetReader.readGeometry("Mario_Triangle.geom", nBytesPerTriangles, mContext));
        mMario.prepare();
        mMario.setTexture(AssetReader.getBitmapFromFile("mario.jpg", mContext), TEXTURE_ID_MARIO);


        mTank = new tank();
        mTank.addGeometry(AssetReader.readGeometry("Tank.geom", nBytesPerTriangles, mContext));
        mTank.prepare();
        mTank.setTexture(AssetReader.getBitmapFromFile("camouflage.jpg", mContext), TEXTURE_ID_TANK);

        mBus = new Bus();
        mBus.addGeometry(AssetReader.readGeometry("Bus.geom", nBytesPerTriangles, mContext));
        mBus.prepare();
//        mBus.setTexture(AssetReader.getBitmapFromFile("grass_tex.jpg", mContext), TEXTURE_ID_BUS);

        mBuilding = new building();
        mBuilding.addGeometry(AssetReader.readGeometry("Building1_vnt.geom", nBytesPerTriangles, mContext));
        mBuilding.prepare();
        mBuilding.setTexture(AssetReader.getBitmapFromFile("grass_tex.jpg", mContext), TEXTURE_ID_BUILDING);


    }

    @Override
    public void onDrawFrame(GL10 gl){ // 그리기 함수 ( = display )
        int pid;
        int timestamp = getTimeStamp();
        float[] modi_view = new float[16], modi_view_2 = new float[16];
        /*
             실시간으로 바뀌는 ViewMatrix의 정보를 가져온다.
             MVP 중 V 매트릭스.
         */
        mViewMatrix = mCamera.GetViewMatrix();
        /*
             fovy 변화를 감지하기 위해 PerspectiveMatrix의 정보를 가져온다.
             MVP 중 P
             mat, offset, fovy, ratio, near, far
         */

        // 눈 좌표계 기준 광원

        for (int i = 0 ; i < 16 ; i++) {
            modi_view[i] = mViewMatrix[i];
        }

        mShadingProgram.set_lights2(modi_view);


        Matrix.perspectiveM(mProjectionMatrix, 0, mCamera.getFovy(), ratio, 0.1f, 2000.0f);

        /*
              행렬 계산을 위해 이제 M만 계산하면 된다.
         */

        GLES30.glClear(GLES30.GL_COLOR_BUFFER_BIT | GLES30.GL_DEPTH_BUFFER_BIT);

        mShadingProgram.set_lights1();

        mShadingProgram.set_lights3();


        rotangle += 1.0f;

        if (rotangle > 360.0f)
            rotangle = 0.0f;


        /*
         그리기 영역.
         */
        mShadingProgram.use(); // 이 프로그램을 사용해 그림을 그릴 것입니다.

        Matrix.setIdentityM(mModelMatrix, 0);

        Matrix.rotateM(mModelMatrix, 0, 90.0f, 1f, 0f, 0f);
        Matrix.rotateM(mModelMatrix, 0, 180.0f, 0f, 1f, 0f);
        Matrix.scaleM(mModelMatrix, 0, 1.0f, 1.0f, 1.0f);
        Matrix.translateM(mModelMatrix, 0, 3 * (float)(Math.cos(Math.toRadians(rotangle))), 3 * (float)(Math.sin(Math.toRadians(rotangle))), 1.0f);
        Matrix.rotateM(mModelMatrix, 0, rotangle, 0f, 0f, 1f);

        Matrix.multiplyMM(mModelViewMatrix, 0, mViewMatrix, 0, mModelMatrix, 0);
        Matrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mModelViewMatrix, 0);
        Matrix.transposeM(mModelViewInvTrans, 0, mModelViewMatrix, 0);
        Matrix.invertM(mModelViewInvTrans, 0, mModelViewInvTrans, 0);

        GLES30.glUniformMatrix4fv(mShadingProgram.locModelViewProjectionMatrix, 1, false, mMVPMatrix, 0);
        GLES30.glUniformMatrix4fv(mShadingProgram.locModelViewMatrix, 1, false, mModelViewMatrix, 0);
        GLES30.glUniformMatrix4fv(mShadingProgram.locModelViewMatrixInvTrans, 1, false, mModelViewInvTrans, 0);

        GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, mMario.mTexId[0]);
        GLES30.glUniform1i(mShadingProgram.locTexture, TEXTURE_ID_MARIO);

        mShadingProgram.setUpMaterialMario();
        mMario.draw();

        // 모델링 좌표계 기준 광원 여기서 setup

        for (int i = 0 ; i < 16 ; i++) {
            modi_view_2[i] = mModelViewMatrix[i];
        }
        mShadingProgram.set_lights4(modi_view_2);


        Matrix.rotateM(mModelMatrix, 0, 90.0f, 1f, 0f, 0f);
        Matrix.scaleM(mModelMatrix, 0, 0.1f, 0.1f, 0.1f);
        Matrix.translateM(mModelMatrix, 0, 0.0f, -7.0f, 0.0f);

        Matrix.multiplyMM(mModelViewMatrix, 0, mViewMatrix, 0, mModelMatrix, 0);
        Matrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mModelViewMatrix, 0);
        Matrix.transposeM(mModelViewInvTrans, 0, mModelViewMatrix, 0);
        Matrix.invertM(mModelViewInvTrans, 0, mModelViewInvTrans, 0);

        GLES30.glUniformMatrix4fv(mShadingProgram.locModelViewProjectionMatrix, 1, false, mMVPMatrix, 0);
        GLES30.glUniformMatrix4fv(mShadingProgram.locModelViewMatrix, 1, false, mModelViewMatrix, 0);
        GLES30.glUniformMatrix4fv(mShadingProgram.locModelViewMatrixInvTrans, 1, false, mModelViewInvTrans, 0);

        mShadingProgram.setUpMaterialBus();
        mBus.draw();



        Matrix.setIdentityM(mModelMatrix, 0);

        Matrix.rotateM(mModelMatrix, 0, -90.0f, 1f, 0f, 0f);
        Matrix.scaleM(mModelMatrix, 0, 0.5f, 0.5f, 0.5f);
        Matrix.translateM(mModelMatrix, 0, 7.0f, -5.0f, 0.0f);

        Matrix.multiplyMM(mModelViewMatrix, 0, mViewMatrix, 0, mModelMatrix, 0);
        Matrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mModelViewMatrix, 0);
        Matrix.transposeM(mModelViewInvTrans, 0, mModelViewMatrix, 0);
        Matrix.invertM(mModelViewInvTrans, 0, mModelViewInvTrans, 0);

        GLES30.glUniformMatrix4fv(mShadingProgram.locModelViewProjectionMatrix, 1, false, mMVPMatrix, 0);
        GLES30.glUniformMatrix4fv(mShadingProgram.locModelViewMatrix, 1, false, mModelViewMatrix, 0);
        GLES30.glUniformMatrix4fv(mShadingProgram.locModelViewMatrixInvTrans, 1, false, mModelViewInvTrans, 0);

        GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, mTank.mTexId[0]);
        GLES30.glUniform1i(mShadingProgram.locTexture, TEXTURE_ID_TANK);

        mShadingProgram.setUpMaterialtank();
        mTank.draw();



        Matrix.setIdentityM(mModelMatrix, 0);

        Matrix.rotateM(mModelMatrix, 0, 90.0f, 1f, 0f, 0f);
        Matrix.scaleM(mModelMatrix, 0, 0.5f, 0.5f, 0.3f);
        Matrix.translateM(mModelMatrix, 0, -100.0f, -100.0f, -35.0f);

        Matrix.multiplyMM(mModelViewMatrix, 0, mViewMatrix, 0, mModelMatrix, 0);
        Matrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mModelViewMatrix, 0);
        Matrix.transposeM(mModelViewInvTrans, 0, mModelViewMatrix, 0);
        Matrix.invertM(mModelViewInvTrans, 0, mModelViewInvTrans, 0);

        GLES30.glUniformMatrix4fv(mShadingProgram.locModelViewProjectionMatrix, 1, false, mMVPMatrix, 0);
        GLES30.glUniformMatrix4fv(mShadingProgram.locModelViewMatrix, 1, false, mModelViewMatrix, 0);
        GLES30.glUniformMatrix4fv(mShadingProgram.locModelViewMatrixInvTrans, 1, false, mModelViewInvTrans, 0);

        GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, mBuilding.mTexId[0]);
        GLES30.glUniform1i(mShadingProgram.locTexture, TEXTURE_ID_BUILDING);

        mShadingProgram.setUpMaterialBuilding();
        mBuilding.draw();


    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height){
        GLES30.glViewport(0, 0, width, height);

        ratio = (float)width / height;

        Matrix.perspectiveM(mProjectionMatrix, 0, mCamera.getFovy(), ratio, 0.1f, 2000.0f);
    }

    static int prevTimeStamp = 0;
    static int currTimeStamp = 0;
    static int totalTimeStamp = 0;

    private int getTimeStamp(){
        Long tsLong = System.currentTimeMillis() / 100;

        currTimeStamp = tsLong.intValue();
        if(prevTimeStamp != 0){
            totalTimeStamp += (currTimeStamp - prevTimeStamp);
        }
        prevTimeStamp = currTimeStamp;

        return totalTimeStamp;
    }

    public void setLight1(){
    //    mShadingProgram.light[1].light_on = 1 - mShadingProgram.light[1].light_on;

    }

    public void setLight2(){
   //     mShadingProgram.light[2].light_on = 1 - mShadingProgram.light[2].light_on;

    }
    public void setLight3(){
   //     mShadingProgram.light[3].light_on = 1 - mShadingProgram.light[3].light_on;

    }
    public void setLight4(){
    //    mShadingProgram.light[4].light_on = 1 - mShadingProgram.light[4].light_on;

    }
}