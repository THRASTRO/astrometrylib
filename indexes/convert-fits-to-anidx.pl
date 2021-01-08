use strict;
use warnings;

################################################################################
################################################################################
# Script to convert all astrometry.net indexes found in the `./fits` folder.
# Converted indexes will be written to the `./andix` folder. That folder
# will not be created for you and the script lacks most error checkings!
# The new format is easier to read by machines than handling fits files.
################################################################################
################################################################################

my $KDT_NULL        = 0;
my $KDT_DATA_NULL   = 0;
my $KDT_DATA_DOUBLE = 0x1;
my $KDT_DATA_FLOAT  = 0x2;
my $KDT_DATA_U32    = 0x4;
my $KDT_DATA_U16    = 0x8;
my $KDT_TREE_NULL   = 0;
my $KDT_TREE_DOUBLE = 0x100;
my $KDT_TREE_FLOAT  = 0x200;
my $KDT_TREE_U32    = 0x400;
my $KDT_TREE_U16    = 0x800;
my $KDT_EXT_NULL    = 0;
my $KDT_EXT_DOUBLE  = 0x10000;
my $KDT_EXT_FLOAT   = 0x20000;

sub kdtree_kdtype_parse_data_string
{
    return $KDT_DATA_NULL unless $_[0];
    if ($_[0] eq "double") {
        return $KDT_DATA_DOUBLE;
    } elsif ($_[0] eq "float") {
        return $KDT_DATA_FLOAT;
    } elsif ($_[0] eq "u32") {
        return $KDT_DATA_U32;
    } elsif ($_[0] eq "u16") {
        return $KDT_DATA_U16;
    } else {
        return $KDT_DATA_NULL;
    }
}

sub kdtree_kdtype_parse_tree_string
{
    return $KDT_TREE_NULL unless $_[0];
    if ($_[0] eq "double") {
        return $KDT_TREE_DOUBLE;
    } elsif ($_[0] eq "float") {
        return $KDT_TREE_FLOAT;
    } elsif ($_[0] eq "u32") {
        return $KDT_TREE_U32;
    } elsif ($_[0] eq "u16") {
        return $KDT_TREE_U16;
    } else {
        return $KDT_TREE_NULL;
    }
}

sub kdtree_kdtype_parse_ext_string
{
    return $KDT_EXT_NULL unless $_[0];
    if ($_[0] eq "double") {
        return $KDT_EXT_DOUBLE;
    } elsif ($_[0] eq "float") {
        return $KDT_EXT_FLOAT;
    } else {
        return $KDT_EXT_NULL;
    }
}

################################################################################

my %table = (
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
);

################################################################################

sub has_header {
    foreach my $line (@{$_[0]->{header}}) {
        if ($line =~ m/^$_[1]\s*=\s*(.*?)\s*(?:\/.*)?$/) {
            return 1;
        }
    }
    return 0;
}

sub get_header {
    foreach my $line (@{$_[0]->{header}}) {
        if ($line =~ m/^$_[1]\s*=\s*(.*?)\s*(?:\/.*)?$/) {
            my $match = $1;
            $match =~ s/\'([^']+?)\s*'/$1/;
            return $match;
        }
    }
    die "$_[1] not found in $_[0]\n";
}

################################################################################

sub process_table {

    my ($oh, $fits) = @_;

    my $ttype = get_header($fits, "TTYPE1"); # kdtree_lr_codes
    my $naxis = get_header($fits, "NAXIS"); # No. of columns in row
    my $naxis1 = get_header($fits, "NAXIS1"); # Bytes in row
    my $naxis2 = get_header($fits, "NAXIS2"); # No. of rows in table
    my $pcount = get_header($fits, "PCOUNT"); # Parameter count always 0
    my $gcount = get_header($fits, "GCOUNT"); # Group count always 1
    my $tfields = get_header($fits, "TFIELDS"); # No. of col in table

    # calculate and check expected sizes
    my $expected = ($naxis1 * $naxis2);
    my $padded = ($naxis1 * $naxis2);
    my $missing = $expected % 2880;
    $padded += 2880 - $missing if ($missing);
    if ($padded ne length($fits->{data})) {
        die "Expected data size does not match";
    }

    # write out the table header
    syswrite($oh, pack("L", $table{$ttype}));
    syswrite($oh, pack("L", $naxis1));
    syswrite($oh, pack("L", $naxis2));

    # write the data as is for this table
    syswrite($oh, substr($fits->{data}, 0, $expected, ""));

    # ensure the rest of data is zero padded
    foreach (unpack("L*", $fits->{data})) {
        die "padded data not zero" if $_;
    }

    warn "Written table $ttype\n";

}

################################################################################

sub process_header {

    my ($oh, $fits) = @_;

    # print out all header cards we have parsed
    # warn join("\n", @{$fits->{header}}), "\n";

    # nothing to do anymore

}

################################################################################

sub process_file {

    my ($file) = @_;

    # open the fits file to parse
    open(my $fh, "<", "fits/$file.fits") or
        die "Could not open $file.fits: $!";
    my $size = -s $fh;

    # simple check since fit files are always multiple of 2880 bytes
    die "Filesize must be multiple of 2880 to be valid fits file" if $size % 2880;

    # calculate number of chunks
    my $fitsChunks = $size / 2880;

    # parse all fits headers and tables
    my @fits; my $header = 1;

    # the current fits table to parse
    my $fit = { header => "", data => "" };

    # read in chunk by chunk and check conditions
    for (my $i = 0; $i < $fitsChunks; $i += 1) {
        # read chunk by chunk and decide
        sysread($fh, my $buffer, 2880);
        # check general fits start condition
        if ($i == 0 && substr($buffer, 0, 6) ne "SIMPLE") {
            die "Header did not start with `SIMPLE`";
        }
        # check start condition for extension table
        if (substr($buffer, 0, 8) eq "XTENSION") {
            # parse all cars (always exactly 80 bytes long)
            $fit->{header} = [unpack("(A80)*", $fit->{header})];
            # store finished table
            push @fits, $fit;
            # create a new table to parse
            $fit = { header => "", data => "" };
            # start with the header
            $header = 1;
        }
        # parse main table data
        if ($header == 1) {
            $fit->{header} .= $buffer;
            if ($buffer =~ m/END\s+$/) {
                $header = 0;
            }
        }
        else {
            $fit->{data} .= $buffer;
        }
    }
    # EO parsing

    # print out all header cards we have parsed
    # warn join("\n", @{$fits[2]->{header}}), "\n";

    # get some meta-data from different tables
    # hopefully order will not change, but code
    # could be adapted (will error if it does).
    my $scale_u = get_header($fits[0], "SCALE_U");
    my $scale_l = get_header($fits[0], "SCALE_L");

    my $healpix = get_header($fits[0], "HEALPIX");
    my $hpnside = get_header($fits[0], "HPNSIDE");

    my $ckdt_name = get_header($fits[2], "KDT_NAME");
    my $ckdt_ndat = get_header($fits[2], "KDT_NDAT");
    my $ckdt_ndim = get_header($fits[2], "KDT_NDIM");
    my $ckdt_nnod = get_header($fits[2], "KDT_NNOD");
    my $ckdt_ver = get_header($fits[2], "KDT_VER");
    my $ckdt_ext = get_header($fits[2], "KDT_EXT");
    my $ckdt_int = get_header($fits[2], "KDT_INT");
    my $ckdt_data = get_header($fits[2], "KDT_DATA");
    my $ckdt_linl = get_header($fits[2], "KDT_LINL");

    my $cext_type = kdtree_kdtype_parse_ext_string($ckdt_ext);
    my $cint_type = kdtree_kdtype_parse_tree_string($ckdt_int);
    my $cdata_type = kdtree_kdtype_parse_data_string($ckdt_data);
    my $ckdt_treetype = $cext_type | $cint_type | $cdata_type;

    my $skdt_name = get_header($fits[7], "KDT_NAME");
    my $skdt_ndat = get_header($fits[7], "KDT_NDAT");
    my $skdt_ndim = get_header($fits[7], "KDT_NDIM");
    my $skdt_nnod = get_header($fits[7], "KDT_NNOD");
    my $skdt_ver = get_header($fits[7], "KDT_VER");
    my $skdt_ext = get_header($fits[7], "KDT_EXT");
    my $skdt_int = get_header($fits[7], "KDT_INT");
    my $skdt_data = get_header($fits[7], "KDT_DATA");
    my $skdt_linl = get_header($fits[7], "KDT_LINL");

    my $sext_type = kdtree_kdtype_parse_ext_string($skdt_ext);
    my $sint_type = kdtree_kdtype_parse_tree_string($skdt_int);
    my $sdata_type = kdtree_kdtype_parse_data_string($skdt_data);
    my $skdt_treetype = $sext_type | $sint_type | $sdata_type;

    my $cxdx = get_header($fits[2], "CXDX");
    my $cxdxlt1 = get_header($fits[2], "CXDXLT1");
    my $circle = get_header($fits[2], "CIRCLE");

    my $cutdedup = get_header($fits[7], "CUTDEDUP");
    my $cutnside = get_header($fits[7], "CUTNSIDE");
    my $cutnswep = get_header($fits[7], "CUTNSWEP");
    my $cutmarg = get_header($fits[7], "CUTMARG");

    my $tables = {};
    # collect the tables for easier lookup
    for (my $i = 1; $i < scalar(@fits); $i += 1) {
        if (has_header($fits[$i], "TTYPE1")) {
            my $ttype = get_header($fits[$i], "TTYPE1");
            $tables->{$ttype} = $fits[$i];
        }
    }

    # open the output file to write to
    open(my $oh, ">", "anidx/$file.anidx") or
        die "Could not open $file.anidx: $!";

    # get indexid from filename
    my $indexid = 9999;
    if ($file =~ m/(\d+)/) {
        $indexid = $1;
    }

    # write the magic header
    syswrite($oh, "anidx01\0");

    # write the main meta-data
    syswrite($oh, pack("L", $healpix));
    syswrite($oh, pack("L", $hpnside));
    syswrite($oh, pack("d", $scale_u));
    syswrite($oh, pack("d", $scale_l));
    syswrite($oh, pack("L", $indexid));
    syswrite($oh, pack("L", $ckdt_ndat));
    syswrite($oh, pack("L", $ckdt_ndim));
    syswrite($oh, pack("L", $ckdt_nnod));
    syswrite($oh, pack("L", $ckdt_treetype));
    syswrite($oh, pack("L", $skdt_ndat));
    syswrite($oh, pack("L", $skdt_ndim));
    syswrite($oh, pack("L", $skdt_nnod));
    syswrite($oh, pack("L", $skdt_treetype));

    syswrite($oh, pack("L", $cutnside));
    syswrite($oh, pack("d", $cutdedup));
    syswrite($oh, pack("L", $cutnswep));
    syswrite($oh, pack("L", $cutmarg));

    syswrite($oh, pack("A", $cxdx));
    syswrite($oh, pack("A", $cxdxlt1));
    syswrite($oh, pack("A", $circle));
    syswrite($oh, pack("A", $ckdt_linl));
    syswrite($oh, pack("A", $skdt_linl));
    syswrite($oh, pack("A", '\0')); # pad6
    syswrite($oh, pack("A", '\0')); # pad7
    syswrite($oh, pack("A", '\0')); # pad8

    # use Fcntl 'SEEK_CUR';
    # my $pos = sysseek($oh, 0, SEEK_CUR);
    # warn("Written $pos\n");

    # process and output all expected tables
    my $kdtree_header_codes = $tables->{"kdtree_header_codes"};
    if ($kdtree_header_codes) { process_header($oh, $kdtree_header_codes); }
    else { warn "Missing table kdtree_header_codes\n" }
    my $kdtree_lr_codes = $tables->{"kdtree_lr_codes"};
    if ($kdtree_lr_codes) { process_table($oh, $kdtree_lr_codes); }
    else { warn "Missing table kdtree_lr_codes\n" }
    my $kdtree_split_codes = $tables->{"kdtree_split_codes"};
    if ($kdtree_split_codes) { process_table($oh, $kdtree_split_codes); }
    else { warn "Missing table kdtree_split_codes\n" }
    my $kdtree_range_codes = $tables->{"kdtree_range_codes"};
    if ($kdtree_range_codes) { process_table($oh, $kdtree_range_codes); }
    else { warn "Missing table kdtree_range_codes\n" }
    my $kdtree_data_codes = $tables->{"kdtree_data_codes"};
    if ($kdtree_data_codes) { process_table($oh, $kdtree_data_codes); }
    else { warn "Missing table kdtree_data_codes\n" }
    my $kdtree_header_stars = $tables->{"kdtree_header_stars"};
    if ($kdtree_header_stars) { process_table($oh, $kdtree_header_stars); }
    else { warn "Missing table kdtree_header_stars\n" }
    my $kdtree_lr_stars = $tables->{"kdtree_lr_stars"};
    if ($kdtree_lr_stars) { process_table($oh, $kdtree_lr_stars); }
    else { warn "Missing table kdtree_lr_stars\n" }
    my $kdtree_split_stars = $tables->{"kdtree_split_stars"};
    if ($kdtree_split_stars) { process_table($oh, $kdtree_split_stars); }
    else { warn "Missing table kdtree_split_stars\n" }
    my $kdtree_range_stars = $tables->{"kdtree_range_stars"};
    if ($kdtree_range_stars) { process_table($oh, $kdtree_range_stars); }
    else { warn "Missing table kdtree_range_stars\n" }
    my $kdtree_data_stars = $tables->{"kdtree_data_stars"};
    if ($kdtree_data_stars) { process_table($oh, $kdtree_data_stars); }
    else { warn "Missing table kdtree_data_stars\n" }
    my $quads = $tables->{"quads"};
    if ($quads) { process_table($oh, $quads); }
    else { warn "Missing table quads\n" }
    my $sweep = $tables->{"sweep"};
    if ($sweep) { process_table($oh, $sweep); }
    else { warn "Missing table sweep\n" }

    # close files
    close($oh);
    close($fh);

}

################################################################################

chdir("4100") or die "4100 folder not found";
opendir(my $dh41, "fits") or die "fits folder not found";
while (my $entry = readdir($dh41)) {
    next unless $entry =~ m/^(.*)\.fits$/;
    warn "#" x 80, "\n";
    warn "Process $1.fits\n";
    warn "#" x 80, "\n";
    process_file($1);
}
closedir($dh41);
chdir("..");

################################################################################

chdir("4200") or die "4200 folder not found";
opendir(my $dh42, "fits") or die "fits folder not found";
while (my $entry = readdir($dh42)) {
    next unless $entry =~ m/^(.*)\.fits$/;
    warn "#" x 80, "\n";
    warn "Process $1.fits\n";
    warn "#" x 80, "\n";
    process_file($1);
}
closedir($dh42);
chdir("..");

################################################################################
