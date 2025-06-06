/*
 *  ssw.h
 *
 *  Created by Mengyao Zhao on 6/22/10.
 *  Copyright 2010 Boston College. All rights reserved.
 *	Version 1.2.3
 *	Last revision by Mengyao Zhao on 2022-May-24.
 *
 */

#ifndef SSW_H
#define SSW_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef __ARM_NEON // (M1)
#include "sse2neon.h"
#else // x86 (Intel)
#include <emmintrin.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

#define MAPSTR "MIDNSHP=X"
#ifndef BAM_CIGAR_SHIFT
#define BAM_CIGAR_SHIFT 4u
#endif

extern const uint8_t encoded_ops[];

/*!	@typedef	structure of the query profile	*/
struct _profile;
typedef struct _profile s_profile;

/*!	@typedef	structure of the alignment result
    @field	score1	the best alignment score
    @field	score2	sub-optimal alignment score
    @field	ref_begin1	0-based best alignment beginning position on reference;	ref_begin1 = -1 when the best alignment beginning
						position is not available
    @field	ref_end1	0-based best alignment ending position on reference
    @field	read_begin1	0-based best alignment beginning position on read; read_begin1 = -1 when the best alignment beginning
						position is not available
    @field	read_end1	0-based best alignment ending position on read
    @field	ref_end2	0-based sub-optimal alignment ending position on reference
    @field	cigar	best alignment cigar; stored the same as that in BAM format, high 28 bits: length, low 4 bits: M/I/D (0/1/2);
					cigar = 0 when the best alignment path is not available
    @field	cigarLen	length of the cigar string; cigarLen = 0 when the best alignment path is not available
    @field  flag  If the alignment path is accurate (or has missing part). 0: accurate; 1: banded_sw is totally failed; 2: banded_sw returned path has missing part
*/
typedef struct {
	uint16_t score1;
	uint16_t score2;
	int32_t ref_begin1;
	int32_t ref_end1;
	int32_t	read_begin1;
	int32_t read_end1;
	int32_t ref_end2;
	uint32_t* cigar;
	int32_t cigarLen;
    uint16_t flag;
} s_align;

/*!	@function	Create the query profile using the query sequence.
	@param	read	pointer to the query sequence; the query sequence needs to be numbers
	@param	readLen	length of the query sequence
	@param	mat	pointer to the substitution matrix; mat needs to be corresponding to the read sequence
	@param	n	the square root of the number of elements in mat (mat has n*n elements)
	@param	score_size	estimated Smith-Waterman score; if your estimated best alignment score is surely < 255 please set 0; if
						your estimated best alignment score >= 255, please set 1; if you don't know, please set 2
	@return	pointer to the query profile structure
	@note	example for parameter read and mat:
			If the query sequence is: ACGTATC, the sequence that read points to can be: 1234142
			Then if the penalty for match is 2 and for mismatch is -2, the substitution matrix of parameter mat will be:
			//A  C  G  T
			  2 -2 -2 -2 //A
			 -2  2 -2 -2 //C
			 -2 -2  2 -2 //G
			 -2 -2 -2  2 //T
			mat is the pointer to the array {2, -2, -2, -2, -2, 2, -2, -2, -2, -2, 2, -2, -2, -2, -2, 2}
*/
s_profile* ssw_init (const int8_t* read, const int32_t readLen, const int8_t* mat, const int32_t n, const int8_t score_size);

/*!	@function	Release the memory allocated by function ssw_init.
	@param	p	pointer to the query profile structure
*/
void init_destroy (s_profile* p);

// @function	ssw alignment.
/*!	@function	Do Striped Smith-Waterman alignment.
	@param	prof	pointer to the query profile structure
	@param	ref	pointer to the target sequence; the target sequence needs to be numbers and corresponding to the mat parameter of
				function ssw_init
	@param	refLen	length of the target sequence
	@param	weight_gapO	the absolute value of gap open penalty
	@param	weight_gapE	the absolute value of gap extension penalty
	@param	flag	bitwise FLAG; (from high to low) bit 5: when setted as 1, function ssw_align will return the best alignment
					beginning position; bit 6: when setted as 1, if (ref_end1 - ref_begin1 < filterd && read_end1 - read_begin1
					< filterd), (whatever bit 5 is setted) the function will return the best alignment beginning position and
					cigar; bit 7: when setted as 1, if the best alignment score >= filters, (whatever bit 5 is setted) the function
  					will return the best alignment beginning position and cigar; bit 8: when setted as 1, (whatever bit 5, 6 or 7 is
 					setted) the function will always return the best alignment beginning position and cigar. When flag == 0, only
					the optimal and sub-optimal scores and the optimal alignment ending position will be returned.
	@param	filters	score filter: when bit 7 of flag is setted as 1 and bit 8 is setted as 0, filters will be used (Please check the
 					decription of the flag parameter for detailed usage.)
	@param	filterd	distance filter: when bit 6 of flag is setted as 1 and bit 8 is setted as 0, filterd will be used (Please check
					the decription of the flag parameter for detailed usage.)
	@param	maskLen	The distance between the optimal and suboptimal alignment ending position >= maskLen. We suggest to use
					readLen/2, if you don't have special concerns. Note: maskLen has to be >= 15, otherwise this function will NOT
					return the suboptimal alignment information. Detailed description of maskLen: After locating the optimal
					alignment ending position, the suboptimal alignment score can be heuristically found by checking the second
					largest score in the array that contains the maximal score of each column of the SW matrix. In order to avoid
					picking the scores that belong to the alignments sharing the partial best alignment, SSW C library masks the
					reference loci nearby (mask length = maskLen) the best alignment ending position and locates the second largest
					score from the unmasked elements.
	@return	pointer to the alignment result structure
	@note	Whatever the parameter flag is setted, this function will at least return the optimal and sub-optimal alignment score,
			and the optimal alignment ending positions on target and query sequences. If both bit 6 and 7 of the flag are setted
			while bit 8 is not, the function will return cigar only when both criteria are fulfilled. All returned positions are
			0-based coordinate.
	@note this has been modified to only return the best alignment - KC 8/31/2022
*/
s_align* ssw_align (const s_profile* prof,
					const int8_t* ref,
					int32_t refLen,
					const uint8_t weight_gapO,
					const uint8_t weight_gapE,
					const uint8_t flag,
					const uint16_t filters,
					const int32_t filterd,
					const int32_t maskLen);

/*!	@function	Release the memory allocated by function ssw_align.
	@param	a	pointer to the alignment result structure
*/
void align_destroy (s_align* a);

/*! @function:
     1. Calculate the number of mismatches.
     2. Modify the cigar string:
         differentiate matches (=), mismatches(X), and softclip(S).
	@param	ref_begin1	0-based best alignment beginning position on the reference sequence
	@param	read_begin1	0-based best alignment beginning position on the read sequence
	@param	read_end1	0-based best alignment ending position on the read sequence
	@param	ref	pointer to the reference sequence
	@param	read	pointer to the read sequence
	@param	readLen	length of the read
	@param	cigar	best alignment cigar; stored the same as that in BAM format, high 28 bits: length, low 4 bits: M/I/D (0/1/2)
	@param	cigarLen	length of the cigar string
 	@return:
     The number of mismatches.
	 The cigar and cigarLen are modified.
*/
int32_t mark_mismatch (int32_t ref_begin1,
					   int32_t read_begin1,
					   int32_t read_end1,
					   const int8_t* ref,
					   const int8_t* read,
					   int32_t readLen,
					   uint32_t** cigar, 
					   int32_t* cigarLen);

/*!	@function		Produce CIGAR 32-bit unsigned integer from CIGAR operation and CIGAR length
	@param	length		length of CIGAR
	@param	op_letter	CIGAR operation character ('M', 'I', etc)
	@return			32-bit unsigned integer, representing encoded CIGAR operation and length
*/
static inline uint32_t to_cigar_int (uint32_t length, char op_letter) {
	return (length << BAM_CIGAR_SHIFT) | (encoded_ops[(int)op_letter]);
}

/*!	@function		Extract CIGAR operation character from CIGAR 32-bit unsigned integer
	@param	cigar_int	32-bit unsigned integer, representing encoded CIGAR operation and length
	@return			CIGAR operation character ('M', 'I', etc)
*/
//char cigar_int_to_op (uint32_t cigar_int);
static inline char cigar_int_to_op(uint32_t cigar_int) {
	return (cigar_int & 0xfU) > 8 ? 'M': MAPSTR[cigar_int & 0xfU];
}

/*!	@function		Extract length of a CIGAR operation from CIGAR 32-bit unsigned integer
	@param	cigar_int	32-bit unsigned integer, representing encoded CIGAR operation and length
	@return			length of CIGAR operation
*/
static inline uint32_t cigar_int_to_len (uint32_t cigar_int) {
	return cigar_int >> BAM_CIGAR_SHIFT;
}

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// SSW_H
