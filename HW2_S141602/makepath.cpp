#define _CRT_SECURE_NO_WARNINGS

#define RATE 0.35
#define HR 12
#define _USE_MATH_DEFINES

#include <stdio.h>
#include <math.h>

int main(){

	double pa[1000][3];
	int i, n;
	int cnt = 0;
	double dx, dy, dz, bi;
	FILE *fp, *fpi;

	int tc, ttc;
	int tx, ty, t2x, t2y, t3x, t3y;
	double rx, ry;


	float drot;
	float srot;
	int rcnt;

	fp = fopen("path.txt", "w");
	fpi = fopen("input.txt", "r");

	fscanf(fpi, "%d", &n);
	for (i = 0; i < n; i++) {
		fscanf(fpi, "%lf %lf %lf", &pa[i][0], &pa[i][1], &pa[i][2]);
	}
	for (i = 1 ; i<n ; i++){
		dx = pa[i][0] - pa[i-1][0];
		dy = pa[i][1] - pa[i-1][1];
		dz = pa[i][2] - pa[i-1][2];
		bi = (dx > 0 ? dx : -dx ) + (dy > 0 ? dy : -dy) + (dz > 0 ? dz : -dz);



		if (i + 1 < n)
		{
			tx = -dx;
			ty = -dy;
			t2x = pa[i + 1][0] - pa[i][0];
			t2y = pa[i + 1][1] - pa[i][1];

			if (tx < 0) tx = -1;
			else if (tx > 0) tx = 1;
			if (ty < 0) ty = -1;
			else if (ty > 0) ty = 1;
			if (t2x < 0) t2x = -1;
			else if (t2x > 0) t2x = 1;
			if (t2y < 0) t2y = -1;
			else if (t2y > 0) t2y = 1;
			t3x = tx + t2x;
			t3y = ty + t2y;
			if (tx > 0) {
				ttc = 1;
				if (t2y > 0) tc = 2;
				else if (t2y < 0) tc = 1;
			}
			else if (tx < 0) {
				ttc = 2;
				if (t2y > 0) tc = 6;
				else if (t2y < 0) tc = 5;
			}
			else if (ty > 0) {
				ttc = 3;
				if (t2x > 0) tc = 8;
				else if (t2x < 0) tc = 7;
			}
			else if (ty < 0) {
				ttc = 4;
				if (t2x > 0) tc = 3;
				else if (t2x < 0) tc = 4;
			}



			rx = pa[i][0] + t3x*HR;
			ry = pa[i][1] + t3y*HR;

			dx = dx / bi;
			dy = dy / bi;
			dz = dz / bi;
			rcnt = 0;
			//fprintf(fp, "%.6lf %.6lf %.6lf\n", pa[i - 1][0], pa[i - 1][1], pa[i - 1][2]);
			//cnt++;

			if ((t3x == 0 && t3y == 0) || t3x >= 2 || t3y >= 2 || t3x <= -2 || t3y <= -2) {
				rcnt = 45;
				t3x = 0;
				t3y = 0;
			}

			switch (ttc) {
			case 1:
				while (pa[i - 1][0] + dx*RATE >= pa[i][0] + t3x*HR) {
					pa[i - 1][0] += dx*RATE;
					pa[i - 1][1] += dy*RATE;
					pa[i - 1][2] += dz*RATE;

					fprintf(fp, "%.6lf %.6lf %.6lf\n", pa[i - 1][0], pa[i - 1][1], pa[i - 1][2]);
					cnt++;
				}
				break;
			case 2:
				while (pa[i - 1][0] + dx*RATE <= pa[i][0] + t3x*HR) {
					pa[i - 1][0] += dx*RATE;
					pa[i - 1][1] += dy*RATE;
					pa[i - 1][2] += dz*RATE;

					fprintf(fp, "%.6lf %.6lf %.6lf\n", pa[i - 1][0], pa[i - 1][1], pa[i - 1][2]);
					cnt++;
				}
				break;
			case 3:
				while (pa[i - 1][1] + dy*RATE >= pa[i][1] + t3y*HR) {
					pa[i - 1][0] += dx*RATE;
					pa[i - 1][1] += dy*RATE;
					pa[i - 1][2] += dz*RATE;

					fprintf(fp, "%.6lf %.6lf %.6lf\n", pa[i - 1][0], pa[i - 1][1], pa[i - 1][2]);
					cnt++;
				}
				break;
			case 4:
				while (pa[i - 1][1] + dy*RATE <= pa[i][1] + t3y*HR) {
					pa[i - 1][0] += dx*RATE;
					pa[i - 1][1] += dy*RATE;
					pa[i - 1][2] += dz*RATE;

					fprintf(fp, "%.6lf %.6lf %.6lf\n", pa[i - 1][0], pa[i - 1][1], pa[i - 1][2]);
					cnt++;
				}
				break;

			}

			if (tc == 1 || tc == 4 || tc == 6 || tc == 8){
				drot = 2;
			}
			else drot = -2;

			if (tc == 1 || tc == 5)
				srot = 90;
			else if (tc == 3 || tc == 8)
				srot = 180;
			else if (tc == 2 || tc == 6)
				srot = 270;
			else
				srot = 0;


			float tttx, ttty;
			while (rcnt < 45) {
				srot += drot;

				tttx = rx + HR * cos(srot*M_PI / 180);
				ttty = ry + HR* sin(srot*M_PI / 180);
				fprintf(fp, "%.6lf %.6lf %.6lf\n", tttx, ttty, pa[i - 1][2]);
				cnt++;
				rcnt++;

				pa[i][0] = tttx;
				pa[i][1] = ttty;
			}


		}

		else {
			dx = dx / bi;
			dy = dy / bi;
			dz = dz / bi;

			if (dx > 0)
				ttc = 2;
			else if (dx < 0)
				ttc = 1;
			else if (dy > 0)
				ttc = 4;
			else if (dy < 0)
				ttc = 3;
			t3x = 0;
			t3y = 0;


			switch (ttc) {
			case 1:
				while (pa[i - 1][0] + dx*RATE >= pa[i][0] + t3x*HR) {
					pa[i - 1][0] += dx*RATE;
					pa[i - 1][1] += dy*RATE;
					pa[i - 1][2] += dz*RATE;

					fprintf(fp, "%.6lf %.6lf %.6lf\n", pa[i - 1][0], pa[i - 1][1], pa[i - 1][2]);
					cnt++;
				}
				break;
			case 2:
				while (pa[i - 1][0] + dx*RATE <= pa[i][0] + t3x*HR) {
					pa[i - 1][0] += dx*RATE;
					pa[i - 1][1] += dy*RATE;
					pa[i - 1][2] += dz*RATE;

					fprintf(fp, "%.6lf %.6lf %.6lf\n", pa[i - 1][0], pa[i - 1][1], pa[i - 1][2]);
					cnt++;
				}
				break;
			case 3:
				while (pa[i - 1][1] + dy*RATE >= pa[i][1] + t3y*HR) {
					pa[i - 1][0] += dx*RATE;
					pa[i - 1][1] += dy*RATE;
					pa[i - 1][2] += dz*RATE;

					fprintf(fp, "%.6lf %.6lf %.6lf\n", pa[i - 1][0], pa[i - 1][1], pa[i - 1][2]);
					cnt++;
				}
				break;
			case 4:
				while (pa[i - 1][1] + dy*RATE <= pa[i][1] + t3y*HR) {
					pa[i - 1][0] += dx*RATE;
					pa[i - 1][1] += dy*RATE;
					pa[i - 1][2] += dz*RATE;

					fprintf(fp, "%.6lf %.6lf %.6lf\n", pa[i - 1][0], pa[i - 1][1], pa[i - 1][2]);
					cnt++;
				}
				break;

			}

		}

	}

	fprintf(fp, "%d", cnt);
	fclose(fpi);
	fclose(fp);
	return 0;
}

