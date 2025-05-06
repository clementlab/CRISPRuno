#!/usr/bin/env python
from setuptools import setup, find_packages

with open('README.md') as readme_file:
    readme = readme_file.read()

requirements = [
        'pandas',
        'biopython',
        'matplotlib',
        'seaborn',
        'jinja2',
        'scipy',
        'numpy',
        'cutadapt',
        ]

test_requirements = [ 
        'pandas',
        'biopython',
        'matplotlib',
        'seaborn',
        'jinja2',
        'scipy',
        'numpy',
        'cutadapt',
        'unittest',
        ]

__version__ = None
with open('src/CRISPRuno/CRISPRunoCore.py') as fin:
    for line in fin:
        if line.startswith('__version__'):
            version_line = line.replace("  "," ")
            version_els = version_line.split(" ")
            __version__ = version_els[2].replace('"','').replace("'","")


setup(
    name='CRISPRuno',
    author="Kendell Clement",
    author_email='k.clement.dev@gmail.com',
    description="CRISPRuno enables analysis of single-anchor amplicon sequencing to quantify complex genome editing outcomes.",
    version=__version__,
    entry_points={
        'console_scripts': [
            'CRISPRuno=CRISPRuno.cli:crispruno',
            'CRISPRunoBatch=CRISPRuno.cli:batch',
            'CRISPRunoCompare=CRISPRuno.cli:compare',
        ],
    },
    install_requires=requirements,
    long_description=readme,
    include_package_data=True,
    keywords='CRISPRuno',
    packages=find_packages(where='src',include=['CRISPRuno', 'CRISPRuno.*']),
    package_dir={'':'src'},
    test_suite='tests',
    tests_require=test_requirements,
    url='https://github.com/kclem/crispruno',
    zip_safe=False,
)
