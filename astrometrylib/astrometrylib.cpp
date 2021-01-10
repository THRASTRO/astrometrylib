#pragma warning (disable : 4996)

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

// Request implementation for unit
#define STB_IMAGE_IMPLEMENTATION

extern "C" {
#include "anidx.h"
#include "solver.h"
#include "onefield.h"
#include "sip-utils.h"
#include "mathutil.h"
#include "stb_image.h"
#include "sep.h"
}

static void add_index(solver_t* sp, const char* fname) {

    index_t* index = (index_t*)malloc(sizeof(index_t));
    memset(index, 0, sizeof(index_t));
    anidx_load(fname, index);
    index->indexfn = strdup(fname);
    solver_add_index(sp, index);
}


// Input values needed to process
// int width = 1944; int height = 1296;
// int nstars = sizeof(starsx) / sizeof(double);

// double starsx[] = { 500.683167, 1242.158081, 985.477905, 1664.199097, 19.704235, 1474.027344, 628.359497, 658.673096, 1526.426514, 1545.087036, 1583.848389, 189.423462, 1761.488647, 1152.949341, 389.818756, 1097.540039, 1085.003296, 999.454163, 798.563477, 1008.911987, 900.140808, 716.133911, 1029.750977, 592.821533, 734.772827, 981.968994, 1027.081787, 489.777069, 1675.119385, 1911.640747, 185.543015, 342.480927, 659.959106, 558.996826, 1532.897949, 502.229980, 440.928497, 298.267731, 1455.319824, 1310.863525, 567.240784, 480.943268, 333.174042, 1449.509277, 492.634460, 1913.880737, 254.856842, 1366.231689, 1601.340698, 350.116669, 1667.941284, 1548.419800, 1267.729858, 1354.994873, 805.972412, 372.067505, 1835.746582, 1463.610962, 1222.551270, 699.566223, 1277.750122, 1675.419312, 1517.063843, 753.418518, 1932.414062, 1609.381958, 469.612244, 644.516235, 1319.641968, 295.847076, 1447.690430, 211.825150, 1241.972656, 845.961487, 442.586182, 1390.663818, 720.758179, 1758.351196, 992.031738, 1061.501343, 1259.796143, 451.444305, 185.978119, 715.997559 };
// double starsy[] = { 11.902996, 31.114763, 49.259823, 80.952766, 93.250870, 97.681374, 100.815140, 133.563690, 145.376846, 144.420090, 145.699203, 154.528824, 174.900070, 191.152252, 193.709579, 374.836670, 982.624207, 463.069244, 471.677460, 557.925659, 816.610291, 786.003601, 757.017944, 646.356445, 604.915283, 583.689270, 732.347107, 209.434769, 226.671463, 282.312500, 322.318298, 325.442505, 346.637329, 368.581604, 373.607452, 417.042664, 426.387207, 458.701050, 472.333160, 479.450958, 576.168945, 549.741943, 623.626465, 643.280212, 641.492065, 663.172424, 679.645142, 683.353821, 716.017822, 726.026428, 730.701904, 735.636353, 783.696350, 822.958130, 834.669800, 874.416748, 874.362671, 942.685974, 973.240784, 991.154114, 1015.974304, 1013.608521, 1075.139404, 1060.234619, 1074.157593, 1086.313843, 1098.776611, 1103.180786, 1118.468872, 1122.958130, 1122.487915, 1142.108032, 1169.877197, 1167.692505, 1176.030640, 1182.999634, 1211.152100, 1219.461182, 1221.418213, 1223.411743, 1230.603516, 1239.920288, 1253.869751, 1279.403809 };

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

// Function to reverse elements of an array
void reverse(const char* arr[], int n)
{
    const char* aux[286];

    for (int i = 0; i < n; i++) {
        aux[n - 1 - i] = arr[i];
    }

    for (int i = 0; i < n; i++) {
        arr[i] = aux[i];
    }
}

// Sample custom data structure for threads to use.
// This is passed by void pointer so it can be any data type
// that can be passed using a single void pointer (LPVOID).
typedef struct MyData {
    int* ntry;
    HANDLE ghMutex;
    bool* cancel;
    int nstars;
    double* starsx;
    double* starsy;
    uint32_t width;
    uint32_t height;
} MYDATA, * PMYDATA;

int main() {

    // Reverse will increase CPU time used
    // Useful for performance tests
    // reverse(indexes, 286);


    int width, height, components;

    const char* fnimg = "../test/field-01.jpg";

    FILE* imgfh;
    fopen_s(&imgfh, fnimg, "rb");
    if (!imgfh) {
        printf("Could not open %s\n", fnimg);
        exit(1);
    }

    // Load the image and assigns width, height and components
    // The components will hold how many components per channel
    // E.g. 3 for rgb images, 4 for rgba images and 1 for monochrome
    stbi_uc* idata = stbi_load_from_file(imgfh, &width, &height, &components, 0);
    if (idata == NULL) {
        printf("Could not load image\n");
        exit(1);
    }
    if (components < 3) {
        printf("Not enough pixel components (rgb expected)\n");
        exit(1);
    }

    fclose(imgfh);

    size_t pixels = width * height;
    // Create monochrome image for the source extraction
    float* mono = (float*)malloc(pixels * sizeof(float));
    for (size_t i = 0, o = 0; o < pixels; i += components, o += 1) {
        mono[o] = (idata[i + 0] + idata[i + 1] + idata[i + 2]) / 3.0;
    }

    // Create the SEP image structure from monochrome data
    sep_image im = { mono, NULL, NULL, NULL, SEP_TFLOAT,
        0, 0, 0, width, height, 0.0, SEP_NOISE_NONE, 1.0, 0.0 };

    sep_bkg* bkg = NULL;
    sep_catalog* catalog = NULL;
    // Get background and subtract it from the image
    int status = sep_background(&im, 64, 64, 3, 3, 0.0, &bkg);
    status = sep_bkg_subarray(bkg, im.data, im.dtype);

    // Get the background data if you want to show it
    float* bgdata = (float*)malloc(pixels * sizeof(float));
    sep_bkg_array(bkg, bgdata, SEP_TFLOAT);

    float conv[] = { 1,2,1, 2,4,2, 1,2,1 };

    // Call the main source extraction algorithm
    // int sep_extract(sep_image * image, float thresh, int thresh_type,
    //     int minarea, float* conv, int convw, int convh,
    //     int filter_type, int deblend_nthresh, double deblend_cont,
    //     int clean_flag, double clean_param,
    //     sep_catalog * *catalog)
    int rv = sep_extract(&im, 10.0 * bkg->globalrms, SEP_THRESH_ABS,
        5, conv, 3, 3, SEP_FILTER_CONV,
        32, 1.0, 1, 1.0, &catalog);

    if (catalog) {
        printf("Extracted %d stars from field-01\n", catalog->nobj);
        starxy_t* starxy = starxy_new(catalog->nobj, TRUE, TRUE);
        for (size_t i = 0; i < catalog->nobj; i += 1) {
            double x = catalog->x[i];
            double y = catalog->y[i];
            printf("Star %zd: %f / %f\n", i,
                catalog->x[i], catalog->y[i]);
        }
    }

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
            printf("EXIT MOTHER\n");
            ExitProcess(2);
        }

        // Generate unique data for each thread to work with.
        pDataArray[i]->ntry = &ntry;
        pDataArray[i]->ghMutex = ghMutex;
        pDataArray[i]->cancel = &cancel;

        pDataArray[i]->width =  width;
        pDataArray[i]->height = height;

        pDataArray[i]->nstars = catalog->nobj;
        pDataArray[i]->starsx = catalog->x;
        pDataArray[i]->starsy = catalog->y;

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

    printf("WAIT FOR SOLVER THREADS\n");

    WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);

    printf("FINSIHED WAITING\n");

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

// RA in degrees to H:M:S
inline void ra2hms(double ra, int* h, int* m, double* s) {
    double rem;
    ra = fmod(ra, 360.0);
    if (ra < 0.0)
        ra += 360.0;
    rem = ra / 15.0;
    *h = floor(rem);
    // remaining (fractional) hours
    rem -= *h;
    // -> minutes
    rem *= 60.0;
    *m = floor(rem);
    // remaining (fractional) minutes
    rem -= *m;
    // -> seconds
    rem *= 60.0;
    *s = rem;
}

// Dec in degrees to D:M:S
void dec2dms(double dec, int* sign, int* d, int* m, double* s) {
    double rem;
    double flr;
    *sign = (dec >= 0.0) ? 1 : -1;
    dec *= (*sign);
    flr = floor(dec);
    *d = flr;
    // remaining degrees:
    rem = dec - flr;
    // -> minutes
    rem *= 60.0;
    *m = floor(rem);
    // remaining (fractional) minutes
    rem -= *m;
    // -> seconds
    rem *= 60.0;
    *s = rem;
}


void ra2hmsstring(double ra, char* str) {
    int h, m;
    double s;
    int ss;
    int ds;
    ra2hms(ra, &h, &m, &s);

    // round to display to 3 decimal places
    ss = (int)floor(s);
    ds = (int)round((s - ss) * 1000.0);
    if (ds >= 1000) {
        ss++;
        ds -= 1000;
    }
    if (ss >= 60) {
        ss -= 60;
        m += 1;
    }
    if (m >= 60) {
        m -= 60;
        h += 1;
    }
    sprintf(str, "%02i:%02i:%02i.%03i", h, m, ss, ds);
}


void dec2dmsstring(double dec, char* str) {
    int sign, d, m;
    double s;
    int ss, ds;
    dec2dms(dec, &sign, &d, &m, &s);
    ss = (int)floor(s);
    ds = (int)round((s - ss) * 1000.0);
    if (ds >= 1000) {
        ss++;
        ds -= 1000;
    }
    if (ss >= 60) {
        ss -= 60;
        m += 1;
    }
    if (m >= 60) {
        m -= 60;
        d += 1;
    }
    sprintf(str, "%c%02i:%02i:%02i.%03i", (sign == 1 ? '+' : '-'), d, m, ss, ds);
}


void sip_get_radec_center_hms_string(const sip_t* wcs,
    char* rastr, char* decstr) {
    double ra, dec;
    sip_get_radec_center(wcs, &ra, &dec);
    ra2hmsstring(ra, rastr);
    dec2dmsstring(dec, decstr);
}



static bool has_distortions(const sip_t* sip) {
    return (sip->a_order >= 0);
}

static void sip_distortion(const sip_t* sip, double x, double y,
    double* X, double* Y) {
    // Get pixel coordinates relative to reference pixel
    double u = x - sip->wcstan.crpix[0];
    double v = y - sip->wcstan.crpix[1];
    sip_calc_distortion(sip, u, v, X, Y);
    *X += sip->wcstan.crpix[0];
    *Y += sip->wcstan.crpix[1];
}

// Pixels to RA,Dec in degrees.
void sip_pixelxy2radec(const sip_t* sip, double px, double py,
    double* ra, double* dec) {
    if (has_distortions(sip)) {
        double U, V;
        sip_distortion(sip, px, py, &U, &V);
        // Run a normal TAN conversion on the distorted pixel coords.
        tan_pixelxy2radec(&(sip->wcstan), U, V, ra, dec);
    }
    else
        // Run a normal TAN conversion
        tan_pixelxy2radec(&(sip->wcstan), px, py, ra, dec);
}

double distsq_between_radecdeg(double ra1, double dec1,
    double ra2, double dec2) {
    double xyz1[3];
    double xyz2[3];
    radecdeg2xyzarr(ra1, dec1, xyz1);
    radecdeg2xyzarr(ra2, dec2, xyz2);
    return distsq(xyz1, xyz2, 3);
}

double arcsec_between_radecdeg(double ra1, double dec1,
    double ra2, double dec2) {
    return distsq2arcsec(distsq_between_radecdeg(ra1, dec1, ra2, dec2));
}

void sip_get_field_size(const sip_t* wcs,
    double* pw, double* ph,
    char** units) {
    double minx = 0.5;
    double maxx = (wcs->wcstan.imagew + 0.5);
    double midx = (minx + maxx) / 2.0;
    double miny = 0.5;
    double maxy = (wcs->wcstan.imageh + 0.5);
    double midy = (miny + maxy) / 2.0;
    double ra1, dec1, ra2, dec2, ra3, dec3;
    double w, h;

    // measure width through the middle
    sip_pixelxy2radec(wcs, minx, midy, &ra1, &dec1);
    sip_pixelxy2radec(wcs, midx, midy, &ra2, &dec2);
    sip_pixelxy2radec(wcs, maxx, midy, &ra3, &dec3);
    w = arcsec_between_radecdeg(ra1, dec1, ra2, dec2) +
        arcsec_between_radecdeg(ra2, dec2, ra3, dec3);
    // measure height through the middle
    sip_pixelxy2radec(wcs, midx, miny, &ra1, &dec1);
    sip_pixelxy2radec(wcs, midx, midy, &ra2, &dec2);
    sip_pixelxy2radec(wcs, midx, maxy, &ra3, &dec3);
    h = arcsec_between_radecdeg(ra1, dec1, ra2, dec2) +
        arcsec_between_radecdeg(ra2, dec2, ra3, dec3);

    if (MIN(w, h) < 60.0) {
        *units = (char*)"arcseconds";
        *pw = w;
        *ph = h;
    }
    else if (MIN(w, h) < 3600.0) {
        *units = (char*)"arcminutes";
        *pw = w / 60.0;
        *ph = h / 60.0;
    }
    else {
        *units = (char*)"degrees";
        *pw = w / 3600.0;
        *ph = h / 3600.0;
    }
}


double tan_get_orientation(const tan_t* tan) {
    double T, A, orient;
    double det, parity;
    det = tan_det_cd(tan);
    parity = (det >= 0 ? 1.0 : -1.0);
    T = parity * tan->cd[0][0] + tan->cd[1][1];
    A = parity * tan->cd[1][0] - tan->cd[0][1];
    orient = -rad2deg(atan2(A, T));
    return orient;
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
                printf("ERROR ReleaseMutex\n");
                // Handle error.
            }
            break;
        case WAIT_ABANDONED:
            return 1;
                printf("WAIT_ABANDONED\n");
            break;
        }

        if (idx >= nindexes) return 0;
        add_index(sp, indexes[idx]);
        printf("Try %s\n", indexes[idx]);

        sp->parity = DEFAULT_PARITY;

        sp->max_cpu_time = 60000;
        sp->max_wall_time = 90000;

        sp->funits_lower = deg2arcsec(0.1) / pDataArray->width;
        sp->funits_upper = deg2arcsec(180.0) / pDataArray->width;

        // gotta keep it to solve it!
        sp->logratio_tokeep = MIN(sp->logratio_tokeep, log(1e9));
        // gotta print it to keep it (so what if that doesn't make sense)!
        sp->logratio_toprint = MIN(sp->logratio_toprint, sp->logratio_tokeep);

        // don't try teeny-tiny quads.
        sp->quadsize_min = 0.1 * MIN(pDataArray->width, pDataArray->height);

        sp->do_tweak = true;
        sp->tweak_aborder = 2; // DEFAULT_TWEAK_ABORDER
        sp->tweak_abporder = 2; // DEFAULT_TWEAK_ABPORDER

        sp->field_maxx = pDataArray->width;
        sp->field_maxy = pDataArray->height;

        sp->cancel = pDataArray->cancel;

        sp->fieldxy_orig = starxy_new(pDataArray->nstars, false, false);
        // sp->fieldxy_orig->N = pDataArray->nstars;
        sp->fieldxy_orig->x = pDataArray->starsx;
        sp->fieldxy_orig->y = pDataArray->starsy;
        // sp->fieldxy_orig->flux = NULL;
        // sp->fieldxy_orig->background = NULL;

        onefield_solve(sp);

        if (sp->best_match_solves) {

            uint64_t delta_cpu = get_cpu_usage() - sp->start_tcpu;
            uint64_t delta_wall = get_wall_time() - sp->start_twall;
            printf("++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
            printf("Field solved in %.2fs (used %.2fs cpu time).\n",
                delta_wall / 1000.0, delta_cpu / 1000.0);
            printf("INDEX: %s\n", sp->index->indexfn);
            printf("++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
            printf("Solve Log Odds:  %f\n", sp->best_logodds);

            double ra, dec, fieldw, fieldh;
            char rastr[32], decstr[32];
            char* fieldunits;
            double orient;

            sip_t& wcs = *sp->best_match.sip;
            sip_get_radec_center(&wcs, &ra, &dec);
            sip_get_radec_center_hms_string(&wcs, rastr, decstr);
            sip_get_field_size(&wcs, &fieldw, &fieldh, &fieldunits);
            orient = tan_get_orientation(&wcs.wcstan);

            printf("Number of Matches:  %d\n", sp->best_match.nmatch);
            printf("Solved with index:  %d\n", sp->best_match.indexid);
            printf("Field center: (RA,Dec) = (%f, %f) deg.\n", ra, dec);
            printf("Field center: (RA H:M:S, Dec D:M:S) = (%s, %s).\n", rastr, decstr);
            printf("Field size: %g x %g %s\n", fieldw, fieldh, fieldunits);
            printf("Field rotation angle: up is %g degrees E of N\n", orient);

            // if (m_UsePosition)
            //     printf(QString("Field is: (%1, %2) deg from search coords.").arg(raErr).arg(decErr));
            // printf(QString("Field size: %1 x %2 %3").arg(fieldw).arg(fieldh).arg(fieldunits));
            // printf(QString("Pixel Scale: %1\"").arg(pixscale));
            // printf(QString("Field rotation angle: up is %1 degrees E of N").arg(orient));
            // printf(QString("Field parity: %1\n").arg(parity));
        }

        solver_cleanup_field(sp);
        anindex_free(sp->index);
        free(sp->index);
        sp->index = 0;

        // Abort if a cancel was requested
        if (sp->cancel && *sp->cancel) {
            printf("CANCEL THREAD\n");
            break;
        }

    }

    printf("FINISHED THREAD\n");

    return 0;
}