FROM mambaorg/micromamba:ubuntu22.04
USER root

ARG MAMBA_DOCKERFILE_ACTIVATE=1

RUN apt-get update && apt-get install gcc g++ bowtie2 samtools \
  -y --no-install-recommends \
  && apt-get clean \
  && rm -rf /var/lib/apt/lists/* \
  && rm -rf /usr/share/man/* \
  && rm -rf /usr/share/doc/* 

RUN micromamba install -c conda-forge -c bioconda -n base -y ncurses python=3.10.11 trimmomatic flash numpy scipy cython cutadapt \
  && micromamba clean --all --yes

RUN micromamba install -c defaults -c conda-forge -c bioconda -y -n base \
      jinja2 matplotlib pandas crispresso2=2.3.1 \
  && micromamba clean --all --yes


RUN micromamba install -c conda-forge -c bioconda -y -n base --debug -c bioconda \
     biopython cas-offinder plotly samtools=1.17 \
  && micromamba clean --all --yes

#install ms fonts
RUN echo "ttf-mscorefonts-installer msttcorefonts/accepted-mscorefonts-eula select true" | debconf-set-selections \
  && apt-get update && apt-get install -y --no-install-recommends \
      fontconfig \
      wget \
      cabextract \
      xfonts-utils \
      ttf-mscorefonts-installer \
  && fc-cache -fv \
  && apt-get clean \
  && apt-get autoremove -y \
  && rm -rf /var/lib/apt/lists/* /usr/share/man/* /usr/share/doc/* /usr/share/zoneinfo

# install crispruno
COPY . /CRISPRuno/
WORKDIR /CRISPRuno
RUN pip install .

ENTRYPOINT ["/usr/local/bin/_entrypoint.sh", "python","/CRISPRuno/src/CRISPRuno/cli.py"]
