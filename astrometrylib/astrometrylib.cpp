#pragma warning (disable : 4996)
#define STRSAFE_NO_DEPRECATE

#include <cassert>
#include <iostream>
#include <limits>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#else
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <dirent.h>
#define FALSE false
#define TRUE true
// code not yet ported to unix
// rest of library should compile
#endif
extern "C" {
#include "compat.h"
}
#include "astrometrylib.hpp"

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

int cmp_str(const void* a, const void* b) {
    char const* aa = *(char const**)a;
    char const* bb = *(char const**)b;
    return strcmp(bb, aa);
}

void load_index_dir(const char* path, char** indexes, int* size)
{
    size_t len = strlen(path);
    char* lookup = (char*)malloc(9 + len);
    if (lookup == 0) exit(1);
    memcpy(lookup, path, len);
    memcpy(lookup + len, "/*.anidx", 9);
#ifdef _WIN32
    HANDLE hFind;
    WIN32_FIND_DATAA FindFileData;
    if ((hFind = FindFirstFileA(lookup, &FindFileData)) != INVALID_HANDLE_VALUE) {
        do {
            size_t fsize = strlen(FindFileData.cFileName);
            size_t need = len + fsize + 2;
            indexes[*size] = (char*)malloc(need);
            if (indexes[*size] == 0) exit(1);
            memcpy((void*)indexes[*size], path, len);
            memcpy((void*)(indexes[*size] + len + 1),
                FindFileData.cFileName, fsize + 1);
            indexes[*size][len] = '/';
            *size += 1;
            if (*size > 1024) {
                printf("Too many indexes\n");
                exit(1);
            }
        } while (FindNextFileA(hFind, &FindFileData));
        FindClose(hFind);
    }

#else

    DIR* dir = opendir(path);
    struct dirent* entry;
    if(dir != NULL)
    {
        while((entry=readdir(dir)) != NULL) {
            size_t fsize = strlen(entry->d_name);
            if (fsize < 7) continue;
            if (entry->d_name[fsize-6] != '.') continue;
            if (entry->d_name[fsize-5] != 'a') continue;
            if (entry->d_name[fsize-4] != 'n') continue;
            if (entry->d_name[fsize-3] != 'i') continue;
            if (entry->d_name[fsize-2] != 'd') continue;
            if (entry->d_name[fsize-1] != 'x') continue;

            size_t need = len + fsize + 2;
            indexes[*size] = (char*)malloc(need);
            if (indexes[*size] == 0) exit(1);
            memcpy((void*)indexes[*size], path, len);
            memcpy((void*)(indexes[*size] + len + 1),
                entry->d_name, fsize + 1);
            indexes[*size][len] = '/';
            *size += 1;
            if (*size > 1024) {
                printf("Too many indexes\n");
                exit(1);
            }
        }
    }
    closedir(dir);

#endif

    free(lookup);

    // for (int i = 0; i < *size; i += 1) {
    //     printf("Index: %s\n", indexes[i]);
    // }
    qsort(indexes, *size, sizeof(char*), cmp_str);

}

int nindexes = 0;
char* indexes[1024];

// Function to reverse elements of an array
void reverse(char* arr[], int n)
{
    char* aux[1024];

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
#ifdef _WIN32
    HANDLE ghMutex;
#else
    pthread_mutex_t ghMutex;
#endif
    bool* cancel;
    int nstars;
    double* starsx;
    double* starsy;
    uint32_t width;
    uint32_t height;
    uint64_t start_tcpu;
    uint64_t start_twall;
} MYDATA, * PMYDATA;

int cmp_sep_flux(const void* a, const void* b)
{
    sep_catalog_idx* idx1 = (sep_catalog_idx*)a;
    sep_catalog_idx* idx2 = (sep_catalog_idx*)b;
    float flux1 = idx1->cat->flux[idx1->idx];
    float flux2 = idx2->cat->flux[idx2->idx];
    return (flux1 < flux2) - (flux1 > flux2);
}

void sep_sort_catalog(sep_catalog* catalog, int (*cmp)(const void* a, const void* b))
{
    sep_catalog_idx* sorted = (sep_catalog_idx*)malloc(catalog->nobj * sizeof(sep_catalog_idx));

    for (int i = 0; i < catalog->nobj; i += 1) {
        sorted[i].cat = catalog;
        sorted[i].idx = i;
    }

    qsort(sorted, catalog->nobj, sizeof(sep_catalog_idx), cmp);

    sep_resort_array(sorted, catalog->nobj, catalog->npix);
    sep_resort_array(sorted, catalog->nobj, catalog->tnpix);
    sep_resort_array(sorted, catalog->nobj, catalog->xmin);
    sep_resort_array(sorted, catalog->nobj, catalog->xmax);
    sep_resort_array(sorted, catalog->nobj, catalog->ymin);
    sep_resort_array(sorted, catalog->nobj, catalog->ymax);
    sep_resort_array(sorted, catalog->nobj, catalog->x);
    sep_resort_array(sorted, catalog->nobj, catalog->y);
    sep_resort_array(sorted, catalog->nobj, catalog->x2);
    sep_resort_array(sorted, catalog->nobj, catalog->y2);
    sep_resort_array(sorted, catalog->nobj, catalog->xy);
    sep_resort_array(sorted, catalog->nobj, catalog->errx2);
    sep_resort_array(sorted, catalog->nobj, catalog->erry2);
    sep_resort_array(sorted, catalog->nobj, catalog->errxy);
    sep_resort_array(sorted, catalog->nobj, catalog->a);
    sep_resort_array(sorted, catalog->nobj, catalog->b);
    sep_resort_array(sorted, catalog->nobj, catalog->theta);
    sep_resort_array(sorted, catalog->nobj, catalog->cxx);
    sep_resort_array(sorted, catalog->nobj, catalog->cyy);
    sep_resort_array(sorted, catalog->nobj, catalog->cxy);
    sep_resort_array(sorted, catalog->nobj, catalog->cflux);
    sep_resort_array(sorted, catalog->nobj, catalog->flux);
    sep_resort_array(sorted, catalog->nobj, catalog->cpeak);
    sep_resort_array(sorted, catalog->nobj, catalog->peak);
    sep_resort_array(sorted, catalog->nobj, catalog->xcpeak);
    sep_resort_array(sorted, catalog->nobj, catalog->ycpeak);
    sep_resort_array(sorted, catalog->nobj, catalog->xpeak);
    sep_resort_array(sorted, catalog->nobj, catalog->ypeak);
    sep_resort_array(sorted, catalog->nobj, catalog->flag);
    sep_resort_array(sorted, catalog->nobj, catalog->pix);
    sep_resort_array(sorted, catalog->nobj, catalog->objectspix);

    free(sorted);
}

int main() {


    load_index_dir("../indexes/4200/anidx",
        indexes, &nindexes);

    // Reverse will increase CPU time used
    // Useful for performance tests
    // reverse(indexes, nindexes);


    int width, height, components;

    const char* fnimg = "../test/field-02.jpg";

    FILE* imgfh = fopen(fnimg, "rb");
    if (!imgfh) {
        logerr("Could not open %s\n", fnimg);
        exit(1);
    }

    // Load the image and assigns width, height and components
    // The components will hold how many components per channel
    // E.g. 3 for rgb images, 4 for rgba images and 1 for monochrome
    stbi_uc* idata = stbi_load_from_file(imgfh, &width, &height, &components, 0);
    if (idata == NULL) {
        logerr("Could not load image\n");
        exit(1);
    }
    if (components < 3) {
        logerr("Not enough pixel components (rgb expected)\n");
        exit(1);
    }
    fclose(imgfh);

    size_t pixels = width * height;
    // Create monochrome image for the source extraction
    float* mono = (float*)malloc(pixels * sizeof(float));
    for (size_t i = 0, o = 0; o < pixels; i += components, o += 1) {
        mono[o] = (idata[i + 0] + idata[i + 1] + idata[i + 2]) / 3.0;
    }

    // Moved data to mono array
    stbi_image_free(idata);

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

    // Free pixel data
    sep_bkg_free(bkg);
    free(bgdata);
    free(mono);

    int cutoff = 255;

    if (catalog) {
        sep_sort_catalog(catalog, cmp_sep_flux);
        if (catalog->nobj < cutoff) cutoff = catalog->nobj;
        logdebug("Extracted %d stars from field-01\n", catalog->nobj);
        // starxy_t* starxy = starxy_new(cutoff, TRUE, TRUE);
        for (size_t i = 0; i < catalog->nobj; i += 1) {
            double x = catalog->x[i];
            double y = catalog->y[i];
            logdebug("Star %zd: %f / %f (%f)\n", i,
                catalog->x[i], catalog->y[i],
                catalog->flux[i]);
        }
    }

    uint64_t start_tcpu = get_cpu_usage(true);
    uint64_t start_twall = get_wall_time();

    // next index entry to process
    int ntry = 0;

    // global flag to cancel threads
    // our usage should be thread-safe
    bool cancel = false;

    PMYDATA pDataArray[MAX_THREADS];
#ifdef _WIN32
    DWORD   dwThreadIdArray[MAX_THREADS];
    HANDLE  hThreadArray[MAX_THREADS];
    // mutex object to access index list
    HANDLE ghMutex = CreateMutex(
        NULL,              // default security attributes
        FALSE,             // initially not owned
        NULL);
#else
    pthread_t hThreadArray[MAX_THREADS];
    // mutex object to access index list
    pthread_mutex_t ghMutex;
    pthread_mutexattr_t ghAttr;
    pthread_mutexattr_init(&ghAttr);
    pthread_mutexattr_settype(&ghAttr, PTHREAD_MUTEX_NORMAL);
    pthread_mutex_init(&ghMutex, &ghAttr);
    pthread_mutexattr_destroy(&ghAttr);
#endif


    // create all threads (not very smart)
    // in reality should e.g. use thread-pool
    for (int i = 0; i < MAX_THREADS; i++)
    {

        // create the object to pass to the thread
        // will pass pointers to our (shared) data
#ifdef _WIN32
        pDataArray[i] = (PMYDATA)HeapAlloc(GetProcessHeap(),
            HEAP_ZERO_MEMORY, sizeof(MYDATA));
#else
        pDataArray[i] = (PMYDATA)calloc(1, sizeof(MYDATA));
#endif

        if (pDataArray[i] == NULL)
        {
            // If the array allocation fails, the system is out of memory
            // so there is no point in trying to print an error message.
            // Just terminate execution.
            logmsg("EXIT MOTHER\n");
#ifdef _WIN32
            ExitProcess(2);
#else
            exit(2);
#endif
        }

        // Generate unique data for each thread to work with.
        pDataArray[i]->ntry = &ntry;
        pDataArray[i]->ghMutex = ghMutex;
        pDataArray[i]->cancel = &cancel;

        pDataArray[i]->width =  width;
        pDataArray[i]->height = height;

        pDataArray[i]->nstars = cutoff;
        pDataArray[i]->starsx = catalog->x;
        pDataArray[i]->starsy = catalog->y;

        pDataArray[i]->start_tcpu = start_tcpu;
        pDataArray[i]->start_twall = start_twall;


#ifdef _WIN32
        // Create the thread to begin execution on its own.
        hThreadArray[i] = CreateThread(
            NULL,                   // default security attributes
            0,                      // use default stack size  
            SolverThread,           // thread function name
            pDataArray[i],          // argument to thread function 
            0,                      // use default creation flags 
            &dwThreadIdArray[i]);   // returns the thread identifier 
#else
        pthread_create(&hThreadArray[i], NULL, SolverThread, pDataArray[i]);
        // pthread_setname_np(hThreadArray[i], "ANLIB-Worker");
#endif

        // Check the return value for success.
        // If CreateThread fails, terminate execution. 
        // This will automatically clean up threads and memory. 
        if (hThreadArray[i] == NULL)
        {
            // ErrorHandler("CreateThread");
#ifdef _WIN32
            ExitProcess(3);
#else
            exit(3);
#endif
        }
    }

    logmsg("WAIT FOR SOLVER THREADS\n");

#ifdef _WIN32
    WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);
#else
    for (int i = 0; i < MAX_THREADS; i++)
    {
        // Would give us back exit value of thread
        pthread_join(hThreadArray[i], NULL);
    }
#endif

    logmsg("FINSIHED WAITING\n");

    // Close all thread handles and free memory allocations.
    for (int i = 0; i < MAX_THREADS; i++)
    {
#ifdef _WIN32
        CloseHandle(hThreadArray[i]);
#else
//        fclose(hThreadArray[i]);
#endif
        if (pDataArray[i] != NULL)
        {
#ifdef _WIN32
            HeapFree(GetProcessHeap(), 0, pDataArray[i]);
#else
            free(pDataArray[i]);
#endif
            pDataArray[i] = NULL;    // Ensure address is not reused.
        }
    }

    uint64_t delta_cpu = get_cpu_usage(true) - start_tcpu;
    uint64_t delta_wall = get_wall_time() - start_twall;
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    printf("Finished after %.2fs (used %.2fs CPU time).\n",
        delta_wall / 1000.0, delta_cpu / 1000.0);
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

    sep_catalog_free(catalog);

    for (size_t i = 0; i < nindexes; i += 1) {
        free(indexes[i]);
    }

    nindexes = 0;

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

void sip_get_radec_center(const sip_t* wcs,
    double* p_ra, double* p_dec) {
    double px = wcs_pixel_center_for_size(wcs->wcstan.imagew);
    double py = wcs_pixel_center_for_size(wcs->wcstan.imageh);
    sip_pixelxy2radec(wcs, px, py, p_ra, p_dec);
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


#ifdef _WIN32
DWORD WINAPI SolverThread(LPVOID lpParam)
#else
void* SolverThread(void* lpParam)
#endif
{

    // Cast the parameter to the correct data type.
    // The pointer is known to be valid because 
    // it was checked for NULL before the thread was created.
    PMYDATA pDataArray = (PMYDATA)lpParam;

    while (true) {

        int idx = 0;

#ifdef _WIN32
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
                logerr("ERROR ReleaseMutex\n");
                // Handle error.
            }
            break;
        case WAIT_ABANDONED:
            return 1;
                logerr("WAIT_ABANDONED\n");
            break;
        }
#else
        if (pthread_mutex_lock(&pDataArray->ghMutex)) {
            logerr("ERROR aquiring lock\n");
        }
        idx = *(pDataArray->ntry);
        *(pDataArray->ntry) += 1;
        if (pthread_mutex_unlock(&pDataArray->ghMutex)) {
            logerr("ERROR releasing lock\n");
        }
#endif
        if (idx >= nindexes) return 0;
        solver_t* sp = solver_new();
        add_index(sp, indexes[idx]);
        logmsg("Try %s\n", indexes[idx]);

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
        // ToDo: Fix this mess
        free(sp->fieldxy_orig->x);
        free(sp->fieldxy_orig->y);
        // sp->fieldxy_orig->N = pDataArray->nstars;
        sp->fieldxy_orig->x = pDataArray->starsx;
        sp->fieldxy_orig->y = pDataArray->starsy;
        // sp->fieldxy_orig->flux = NULL;
        // sp->fieldxy_orig->background = NULL;

        onefield_solve(sp);

        uint64_t delta_cpu = get_cpu_usage(true) - pDataArray->start_tcpu;
        uint64_t delta_wall = get_wall_time() - pDataArray->start_twall;

        if (sp->best_match_solves) {

            printf("++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
            printf("Field solved in %.2fs (used %.2fs CPU time).\n",
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
            printf("####################################################\n");
            printf("Field center: (RA, Dec) = (%f, %f) deg.\n", ra, dec);
            printf("Field center: (RA H:M:S, Dec D:M:S) = (%s, %s).\n", rastr, decstr);
            printf("Field size: %g x %g %s\n", fieldw, fieldh, fieldunits);
            printf("Field rotation angle: up is %g degrees E of N\n", orient);
            printf("####################################################\n");

            sip_free(sp->best_match.sip);
            // verify_free_matchobj(&sp->best_match);

            // if (m_UsePosition)
            //     printf(QString("Field is: (%1, %2) deg from search coords.").arg(raErr).arg(decErr));
            // printf(QString("Pixel Scale: %1\"").arg(pixscale));
            // printf(QString("Field parity: %1\n").arg(parity));
        }

        solver_cleanup_field(sp);
        anindex_free(sp->index);
        free(sp->index);
        sp->index = 0;

        // Abort if a cancel was requested
        if (sp->cancel && *sp->cancel) {
            logmsg("CANCEL THREAD\n");
            solver_cleanup(sp);
            solver_free(sp);
            break;
        }

        solver_cleanup(sp);
        solver_free(sp);

    }

    logmsg("FINISHED THREAD\n");

#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
}

