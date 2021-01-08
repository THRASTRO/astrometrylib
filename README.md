# AstrometryLib

Stripped down astrometry.net star plate solver

## Overview

[Astrometry.net][1] is an incredibly good star plate solver, but it comes in the form
of a command-line application. Without any real programming API for third parties to
directly utilize the algorithm. This is specially true if you want to compile the
code on windows, since the original code uses quite a bit of non-standard posix code.
This project tries to solve the interface problem and compatibility with MSVC.

There exists another project named [stellarsolver][2] that tries to do the same, but
I already was half way through the code when I found it. I also took another approach
with the index files. See chapter "binary indexes" below.

## Binary 'anidx' indexes

While porting the code I soon decided that I want to get rid of the `qfits` dependency.
The `fits` format may be well to interchange stuff it seems far less ideal for something
like a lookup index, which could be perfectly and easily stored in a binary form.

Although the data in the index `fits` tables are also just in binary form, the parsing of
the various options within the `fits` cards was more than just tedious. Also the used
`qfits` library was less than idea to port to windows. Therefore it was way easier to
just convert the `fits` indexes to a binary format which I call `anidx`. Note that the
index will only work if it matches your machine' endianness (most machines today are
little-endian, e.g. x86 or x64). The format is not really final (I should probably
add an endianness test).

The following is the header structure (96 bytes). Note that the order is very important
to get the memory alignment right. I believe the current order should work well.

```c
struct anidx_header {

    char magic[8];

    uint32_t healpix;
    uint32_t hpnside;

    double scale_u;
    double scale_l;

    uint32_t indexid;

    uint32_t ckdt_ndat;
    uint32_t ckdt_ndim;
    uint32_t ckdt_nnod;
    uint32_t ckdt_type;

    uint32_t skdt_ndat;
    uint32_t skdt_ndim;
    uint32_t skdt_nnod;
    uint32_t skdt_type;

    uint32_t cutnside;
    double cutdedup;
    uint32_t cutnswep;
    uint32_t cutmarg;

    char cxdx;
    char cxdxlt1;
    char circle;
    char ckdt_linl;
    char skdt_linl;
    char padded6;
    char padded7;
    char padded8;

};
```

Following the header we expect exactly 11 tables:

```
    'kdtree_header_codes' => 0,
    'kdtree_lr_codes' => 1,
    'kdtree_split_codes' => 2,
    'kdtree_range_codes' => 3,
    'kdtree_data_codes' => 4,
    'kdtree_header_stars' => 5,
    'kdtree_lr_stars' => 6,
    'kdtree_split_stars' => 7,
    'kdtree_range_stars' => 8,
    'kdtree_data_stars' => 9,
    'quads' => 10,
    'sweep' => 11,
```

Each table stars with its individual header:

```c
struct anidx_table_header {

    uint32_t type;
    uint32_t naxis1;
    uint32_t naxis2;

};
```

Size for the data is `naxis1 * naxis2`.

## Convert 'fits' indexes

There is a perl-script `indexes/convert-fits-to-anidx.pl` that does the converting.
It's not very versatile and has a pretty dumb "hand-crafted" fits parsing.
Currently it can't convert endianness, although that shouldn't be too hard.
Alternatively I made them [available for download][3].

## Current Status

This is a bit further than POC status currently.

- There is a small demo application with solution file for MSVC.
- Star positions are hardcoded (taken from an .xyls file).
- You will need to download or generate the binary indexes.
- Will use 12 parallel threads (checks one index per thread).
- CPU and real time limits for windows have been implemented.
- There are probably some leaks left, although no big ones.

### How does it all work?

If you wonder how the plate solving internally works, I tried to
document some of my findings at [astronomy stackexchange][4].


[1]: http://astrometry.net/
[2]: https://github.com/rlancaste/stellarsolver
[3]: http://www.ocbnet.ch/astrometrylib/
[4]: https://astronomy.stackexchange.com/questions/33575/how-the-heck-does-astrometry-net-work/40644#40644