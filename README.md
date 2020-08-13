# Disscalc
`disscalc` is a simple tool for generating dissonance curves in the form of
delimiter-separated values.

## Usage
### Output
By default, `disscalc` will output the table as CSV. Using the
`--format=<format>` or `-f <format>` options, it is also possible to generate
TSV.

For example, the command

    disscalc -p 300 400 -a 10 20 -d 0.2

or

    disscalc -p 300 400 -a 10 20 -d 0.2 --format=csv

will produce the output

    1,3.10373
    1.2,19.4305
    1.4,10.3045
    1.6,3.52191
    1.8,0.63834
    2,0.103617

However, with the command

    disscalc -p 300 400 -a 10 20 -d 0.2 --format=tsv

the output will be

    1	3.10373
    1.2	19.4305
    1.4	10.3045
    1.6	3.52191
    1.8	0.63834
    2	0.103617

Additionally, the output can be written to a file using `--output=<file>` or
`-o <file>`.

### Input intervals
`disscalc` will compute the dissonance of most ranges of intervals. Each
interval from 1.0 to 2.0 will be used as input, with a distance of 0.01 between
each one. The lower bound can be changed with `-s <interval>` or
`--start=<interval>`. `--end` or `-e` and `--delta` or `-d` can be used in the
same way for the upper bound and the distance between intervals, respectively.

For example

    disscalc -s 10 -d 2 -e 18 -f tsv

will produce

    10	0
    12	0
    14	0
    16	0
    18	0

while

    disscalc -s 0.5 -d 0.3 -e 0.9 -f tsv

will produce

    0.5	0
    0.8	0

Note that the upper bound is only used as an input if the lower bound plus some
natural number multiple of the delta equals it exactly.

If there are any other values outside of the provided range that should also be
used as inputs, they can be specified using the `-x` option. For example,

    dissclac -s 1 -d 0.2 -e 2 -x 0.1294 1.5 98.6 -f tsv

will result in the output

    0.1294	0
    1	0
    1.2	0
    1.4	0
    1.5	0
    1.6	0
    1.8	0
    2	0
    98.6	0

### Timbres
Timbres are provided as a list of their partials, each of which is has a
frequency and an amplitude. The `-p` option is used to provide the frequencies,
and the amplitudes are provided in the same order using the `-a` option. When
invoked as

    disscalc -p 4502.98 34 -a 9.6 27.5

the timbre will consist of two partials:
- One with a frequency of 4502.98 and an amplitude of 9.6
- Another with a frequency of 34 and an amplitude of 27.5

Normally, this timbre will be used for both notes in the interval. It is also
possible to specify a different timbre for the note raised by the interval,
using the `-P` and `-A` options in the same way.
