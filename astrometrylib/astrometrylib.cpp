#include <cassert>
#include <iostream>
#include <limits>
#include <vector>

#include <windows.h>
#include <tchar.h>
#include <strsafe.h>

#define MAX_THREADS 12
#define BUF_SIZE 255

DWORD WINAPI SolverThread(LPVOID lpParam);

extern "C" {
#include "anidx.h"
#include "solver.h"
#include "onefield.h"
}

static void add_index(solver_t* sp, const char* fname) {

    index_t* index = (index_t*)malloc(sizeof(index_t));
    memset(index, 0, sizeof(index_t));
    anidx_load(fname, index);
    index->indexfn = strdup(fname);
    solver_add_index(sp, index);
}


double starsx[] = { 500.683167, 1242.158081, 985.477905, 1664.199097, 19.704235, 1474.027344, 628.359497, 658.673096, 1526.426514, 1545.087036, 1583.848389, 189.423462, 1761.488647, 1152.949341, 389.818756, 1097.540039, 1085.003296, 999.454163, 798.563477, 1008.911987, 900.140808, 716.133911, 1029.750977, 592.821533, 734.772827, 981.968994, 1027.081787, 489.777069, 1675.119385, 1911.640747, 185.543015, 342.480927, 659.959106, 558.996826, 1532.897949, 502.229980, 440.928497, 298.267731, 1455.319824, 1310.863525, 567.240784, 480.943268, 333.174042, 1449.509277, 492.634460, 1913.880737, 254.856842, 1366.231689, 1601.340698, 350.116669, 1667.941284, 1548.419800, 1267.729858, 1354.994873, 805.972412, 372.067505, 1835.746582, 1463.610962, 1222.551270, 699.566223, 1277.750122, 1675.419312, 1517.063843, 753.418518, 1932.414062, 1609.381958, 469.612244, 644.516235, 1319.641968, 295.847076, 1447.690430, 211.825150, 1241.972656, 845.961487, 442.586182, 1390.663818, 720.758179, 1758.351196, 992.031738, 1061.501343, 1259.796143, 451.444305, 185.978119, 715.997559 };
double starsy[] = { 11.902996, 31.114763, 49.259823, 80.952766, 93.250870, 97.681374, 100.815140, 133.563690, 145.376846, 144.420090, 145.699203, 154.528824, 174.900070, 191.152252, 193.709579, 374.836670, 982.624207, 463.069244, 471.677460, 557.925659, 816.610291, 786.003601, 757.017944, 646.356445, 604.915283, 583.689270, 732.347107, 209.434769, 226.671463, 282.312500, 322.318298, 325.442505, 346.637329, 368.581604, 373.607452, 417.042664, 426.387207, 458.701050, 472.333160, 479.450958, 576.168945, 549.741943, 623.626465, 643.280212, 641.492065, 663.172424, 679.645142, 683.353821, 716.017822, 726.026428, 730.701904, 735.636353, 783.696350, 822.958130, 834.669800, 874.416748, 874.362671, 942.685974, 973.240784, 991.154114, 1015.974304, 1013.608521, 1075.139404, 1060.234619, 1074.157593, 1086.313843, 1098.776611, 1103.180786, 1118.468872, 1122.958130, 1122.487915, 1142.108032, 1169.877197, 1167.692505, 1176.030640, 1182.999634, 1211.152100, 1219.461182, 1221.418213, 1223.411743, 1230.603516, 1239.920288, 1253.869751, 1279.403809 };

// Input values needed to process
int width = 1944; int height = 1296;
int nstars = sizeof(starsx) / sizeof(double);

int nindexes = 286;
const char* indexes[286] = {
    "..\\indexes\\4200\\anidx\\index-4219.anidx",
    "..\\indexes\\4200\\anidx\\index-4218.anidx",
    "..\\indexes\\4200\\anidx\\index-4217.anidx",
    "..\\indexes\\4200\\anidx\\index-4216.anidx",
    "..\\indexes\\4200\\anidx\\index-4215.anidx",
    "..\\indexes\\4200\\anidx\\index-4214.anidx",
    "..\\indexes\\4200\\anidx\\index-4213.anidx",
    "..\\indexes\\4200\\anidx\\index-4212.anidx",
    "..\\indexes\\4200\\anidx\\index-4211.anidx",
    "..\\indexes\\4200\\anidx\\index-4210.anidx",
    "..\\indexes\\4200\\anidx\\index-4209.anidx",
    "..\\indexes\\4200\\anidx\\index-4208.anidx",

    "..\\indexes\\4200\\anidx\\index-4207-00.anidx",
    "..\\indexes\\4200\\anidx\\index-4207-01.anidx",
    "..\\indexes\\4200\\anidx\\index-4207-02.anidx",
    "..\\indexes\\4200\\anidx\\index-4207-03.anidx",
    "..\\indexes\\4200\\anidx\\index-4207-04.anidx",
    "..\\indexes\\4200\\anidx\\index-4207-05.anidx",
    "..\\indexes\\4200\\anidx\\index-4207-06.anidx",
    "..\\indexes\\4200\\anidx\\index-4207-07.anidx",
    "..\\indexes\\4200\\anidx\\index-4207-08.anidx",
    "..\\indexes\\4200\\anidx\\index-4207-09.anidx",
    "..\\indexes\\4200\\anidx\\index-4207-10.anidx",
    "..\\indexes\\4200\\anidx\\index-4207-11.anidx",

    "..\\indexes\\4200\\anidx\\index-4206-00.anidx",
    "..\\indexes\\4200\\anidx\\index-4206-01.anidx",
    "..\\indexes\\4200\\anidx\\index-4206-02.anidx",
    "..\\indexes\\4200\\anidx\\index-4206-03.anidx",
    "..\\indexes\\4200\\anidx\\index-4206-04.anidx",
    "..\\indexes\\4200\\anidx\\index-4206-05.anidx",
    "..\\indexes\\4200\\anidx\\index-4206-06.anidx",
    "..\\indexes\\4200\\anidx\\index-4206-07.anidx",
    "..\\indexes\\4200\\anidx\\index-4206-08.anidx",
    "..\\indexes\\4200\\anidx\\index-4206-09.anidx",
    "..\\indexes\\4200\\anidx\\index-4206-10.anidx",
    "..\\indexes\\4200\\anidx\\index-4206-11.anidx",

    "..\\indexes\\4200\\anidx\\index-4205-00.anidx",
    "..\\indexes\\4200\\anidx\\index-4205-01.anidx",
    "..\\indexes\\4200\\anidx\\index-4205-02.anidx",
    "..\\indexes\\4200\\anidx\\index-4205-03.anidx",
    "..\\indexes\\4200\\anidx\\index-4205-04.anidx",
    "..\\indexes\\4200\\anidx\\index-4205-05.anidx",
    "..\\indexes\\4200\\anidx\\index-4205-06.anidx",
    "..\\indexes\\4200\\anidx\\index-4205-07.anidx",
    "..\\indexes\\4200\\anidx\\index-4205-08.anidx",
    "..\\indexes\\4200\\anidx\\index-4205-09.anidx",

    "..\\indexes\\4200\\anidx\\index-4204-00.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-01.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-02.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-03.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-04.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-05.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-06.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-07.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-08.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-09.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-10.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-11.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-12.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-13.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-14.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-15.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-16.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-17.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-18.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-19.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-20.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-21.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-22.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-23.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-24.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-25.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-26.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-27.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-28.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-29.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-30.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-31.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-32.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-33.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-34.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-35.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-36.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-37.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-38.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-39.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-40.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-41.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-42.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-43.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-44.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-45.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-46.anidx",
    "..\\indexes\\4200\\anidx\\index-4204-47.anidx",

    "..\\indexes\\4200\\anidx\\index-4203-00.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-01.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-02.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-03.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-04.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-05.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-06.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-07.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-08.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-09.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-10.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-11.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-12.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-13.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-14.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-15.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-16.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-17.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-18.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-19.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-20.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-21.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-22.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-23.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-24.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-25.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-26.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-27.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-28.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-29.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-30.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-31.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-32.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-33.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-34.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-35.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-36.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-37.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-38.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-39.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-40.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-41.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-42.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-43.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-44.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-45.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-46.anidx",
    "..\\indexes\\4200\\anidx\\index-4203-47.anidx",

    "..\\indexes\\4200\\anidx\\index-4202-00.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-01.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-02.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-03.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-04.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-05.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-06.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-07.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-08.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-09.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-10.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-11.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-12.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-13.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-14.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-15.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-16.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-17.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-18.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-19.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-20.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-21.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-22.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-23.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-24.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-25.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-26.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-27.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-28.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-29.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-30.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-31.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-32.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-33.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-34.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-35.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-36.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-37.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-38.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-39.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-40.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-41.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-42.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-43.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-44.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-45.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-46.anidx",
    "..\\indexes\\4200\\anidx\\index-4202-47.anidx",

    "..\\indexes\\4200\\anidx\\index-4201-00.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-01.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-02.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-03.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-04.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-05.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-06.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-07.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-08.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-09.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-10.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-11.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-12.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-13.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-14.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-15.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-16.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-17.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-18.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-19.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-20.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-21.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-22.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-23.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-24.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-25.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-26.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-27.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-28.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-29.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-30.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-31.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-32.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-33.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-34.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-35.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-36.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-37.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-38.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-39.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-40.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-41.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-42.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-43.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-44.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-45.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-46.anidx",
    "..\\indexes\\4200\\anidx\\index-4201-47.anidx",

    "..\\indexes\\4200\\anidx\\index-4200-00.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-01.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-02.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-03.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-04.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-05.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-06.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-07.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-08.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-09.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-10.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-11.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-12.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-13.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-14.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-15.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-16.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-17.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-18.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-19.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-20.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-21.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-22.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-23.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-24.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-25.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-26.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-27.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-28.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-29.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-30.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-31.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-32.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-33.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-34.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-35.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-36.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-37.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-38.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-39.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-40.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-41.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-42.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-43.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-44.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-45.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-46.anidx",
    "..\\indexes\\4200\\anidx\\index-4200-47.anidx"

};

// Sample custom data structure for threads to use.
// This is passed by void pointer so it can be any data type
// that can be passed using a single void pointer (LPVOID).
typedef struct MyData {
    int* ntry;
    HANDLE ghMutex;
    bool* cancel;
} MYDATA, * PMYDATA;

int main() {

    PMYDATA pDataArray[MAX_THREADS];
    DWORD   dwThreadIdArray[MAX_THREADS];
    HANDLE  hThreadArray[MAX_THREADS];

    // next index entry to process
    int ntry = 0;

    // global flag to cancel threads
    // our usage should be thread-safe
    bool cancel = false;

    // mutex object to access index list
    HANDLE ghMutex = CreateMutex(
        NULL,              // default security attributes
        FALSE,             // initially not owned
        NULL);

    // create all threads (not very smart)
    // in reality should e.g. use thread-pool
    for (int i = 0; i < MAX_THREADS; i++)
    {

        // create the object to pass to the thread
        // will pass pointers to our (shared) data
        pDataArray[i] = (PMYDATA)HeapAlloc(GetProcessHeap(),
            HEAP_ZERO_MEMORY, sizeof(MYDATA));

        if (pDataArray[i] == NULL)
        {
            // If the array allocation fails, the system is out of memory
            // so there is no point in trying to print an error message.
            // Just terminate execution.
            ExitProcess(2);
        }

        // Generate unique data for each thread to work with.
        pDataArray[i]->ntry = &ntry;
        pDataArray[i]->ghMutex = ghMutex;
        pDataArray[i]->cancel = &cancel;

        // Create the thread to begin execution on its own.
        hThreadArray[i] = CreateThread(
            NULL,                   // default security attributes
            0,                      // use default stack size  
            SolverThread,           // thread function name
            pDataArray[i],          // argument to thread function 
            0,                      // use default creation flags 
            &dwThreadIdArray[i]);   // returns the thread identifier 


        // Check the return value for success.
        // If CreateThread fails, terminate execution. 
        // This will automatically clean up threads and memory. 
        if (hThreadArray[i] == NULL)
        {
            // ErrorHandler("CreateThread");
            ExitProcess(3);
        }
    }

    WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);

    // Close all thread handles and free memory allocations.
    for (int i = 0; i < MAX_THREADS; i++)
    {
        CloseHandle(hThreadArray[i]);
        if (pDataArray[i] != NULL)
        {
            HeapFree(GetProcessHeap(), 0, pDataArray[i]);
            pDataArray[i] = NULL;    // Ensure address is not reused.
        }
    }

    return 0;

}

DWORD WINAPI SolverThread(LPVOID lpParam)
{

    // Cast the parameter to the correct data type.
    // The pointer is known to be valid because 
    // it was checked for NULL before the thread was created.
    PMYDATA pDataArray = (PMYDATA)lpParam;

    while (true) {

        int idx = 0;
        solver_t* sp = solver_new();

        // Acquire exclusive access to globals
        DWORD dwWaitResult = WaitForSingleObject(
            pDataArray->ghMutex, // handle to mutex
            INFINITE);  // no time-out interval

        switch (dwWaitResult)
        {
        case WAIT_OBJECT_0:
            idx = *(pDataArray->ntry);
            *(pDataArray->ntry) += 1;
            if (!ReleaseMutex(pDataArray->ghMutex))
            {
                // Handle error.
            }
            break;
        case WAIT_ABANDONED:
            return 1;
            break;
        }

        if (idx >= nindexes) return 0;
        add_index(sp, indexes[idx]);
        printf("Try %s\n", indexes[idx]);

        sp->parity = DEFAULT_PARITY;

        sp->max_cpu_time = 60000;
        sp->max_wall_time = 90000;

        sp->funits_lower = deg2arcsec(0.1) / width;
        sp->funits_upper = deg2arcsec(180.0) / width;

        // gotta keep it to solve it!
        sp->logratio_tokeep = MIN(sp->logratio_tokeep, log(1e9));
        // gotta print it to keep it (so what if that doesn't make sense)!
        sp->logratio_toprint = MIN(sp->logratio_toprint, sp->logratio_tokeep);

        // don't try teeny-tiny quads.
        sp->quadsize_min = 0.1 * MIN(width, height);

        sp->do_tweak = true;
        sp->tweak_aborder = 2; // DEFAULT_TWEAK_ABORDER
        sp->tweak_abporder = 2; // DEFAULT_TWEAK_ABPORDER

        sp->field_maxx = width;
        sp->field_maxy = height;

        sp->cancel = pDataArray->cancel;

        sp->fieldxy_orig = starxy_new(nstars, false, false);
        sp->fieldxy_orig->x = starsx;
        sp->fieldxy_orig->y = starsy;
        // sp->fieldxy_orig->flux = NULL;
        // sp->fieldxy_orig->background = NULL;
        sp->fieldxy_orig->N = nstars;

        onefield_solve(sp);

        // Abort if a cancel was requested
        if (sp->cancel && *sp->cancel) break;

    }

    return 0;
}