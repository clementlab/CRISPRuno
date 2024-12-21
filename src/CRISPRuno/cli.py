"""Console script for crispruno."""
import argparse
import logging
import sys
from CRISPRuno.CRISPRunoCore import parse_settings, processCRISPRuno
from CRISPRuno.CRISPRunoBatch import processBatch
from CRISPRuno.CRISPRunoCompare import compareCRISPRuno


def main():
    if len(sys.argv) == 1:
        sys.argv[0] = "CRISPRuno"
        crispruno()
    if sys.argv[1] == "CRISPRuno":
        sys.argv.remove("CRISPRuno")
        sys.argv[0] = "CRISPRuno"
        crispruno()
    elif sys.argv[1] == "CRISPRunoBatch":
        sys.argv.remove('CRISPRunoBatch')
        sys.argv[0] = 'CRISPRunoBatch'
        batch()
    elif sys.argv[1] == "CRISPRunoCompare":
        sys.argv.remove('CRISPRunoCompare')
        sys.argv[0] = 'CRISPRunoCompare'
        compare()
    else:
        crispruno()



def crispruno():
    if len(sys.argv) == 1:
        print('CRISPRuno: Analyzing unidirectional sequencing of genome editing\n' + \
            'usage: CRISPRuno --fastq_r1 r1.fq --genome hg38.fa --guide_sequences ATTA --primer_seq GCTA\n' + \
            'commonly-used arguments:\n' + \
            '-h, --help            show the full list of arguments\n' + \
            '-v, --version         show program\'s version number and exit\n' + \
            '--fastq_r1            FASTQ file containing the reads to analyze (required)\n' + \
            '--genome              genome sequence file in FASTA format (required)\n' + \
            '--guide_sequences     guide sequences (may be specified multiple times)\n' + \
            '--primer_seq          primer sequence\n\n' + \
            'Alternately, these arguments may be specified in a settings file\n' + \
            '     which is a tab-separated file of setting names and values.\n'

        )
        sys.exit()
    try:
        settings = parse_settings(sys.argv)
        processCRISPRuno(settings)
    except Exception as e:
        logger = logging.getLogger('CRISPRuno')
        if logger.isEnabledFor(logging.DEBUG):
            logger.error(e, exc_info=True)
        else:
            logger.error(e)
        if '--debug' in sys.argv:
            raise e
        else:
            print(str(e))
            sys.exit(1)

def batch():
    try:
        processBatch(sys.argv)
    except Exception as e:
        if '--debug' in sys.argv:
            raise e
        else:
            print(str(e))
            sys.exit(1)

def compare():
    try:
        compareCRISPRuno(sys.argv)
    except Exception as e:
        if '--debug' in sys.argv:
            raise e
        else:
            print(e)
            sys.exit(1)

if __name__ == "__main__":
    sys.exit(main())
