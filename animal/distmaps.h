/*
 * =============================================================================
 *
 * D I S T A N C E   T R A N S F O R M   R O U T I N E S  --  HEADER
 *
 * $Revision: 1.4 $ $Date: 2009-03-29 04:10:18 $
 *
 * ANIMAL - ANIMAL IMage Processing LibrarY
 * Copyright (C) 2002,2003-2011  Ricardo Fabbri <rfabbri@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * =============================================================================
 */ 
#ifndef DT_H
#define DT_H 1

#include "common.h"
#include "img.h"
#include "adjacency.h"

BEGIN_C_DECLS

// Dinstance Transform algorithm options
typedef enum {
   DT_LOTUFO_ZAMPIROLLI, // R. Lotufo & F. Zampirolli 1999 algorithm 
                         // (exact; very fast)

   DT_MAURER2003,            // C. Maurer et. al. 2003 algorithm (exact, very fast)

   DT_MEIJSTER_2000,         // A. Meijster et. al. 2000 algorithm (exact, very fast)

   DT_CUISENAIRE_PMN_1999,   // O. Cuisenaire CVIU 1999 (exact, very fast)
   DT_CUISENAIRE_PMON_1999,  // O. Cuisenaire CVIU 1999 (exact, very fast)
   DT_CUISENAIRE_PSN4_1999,  // Inexact, very fast
   DT_CUISENAIRE_PSN8_1999,  // A little less inexact, very fast

   DT_SAITO_1994,        // Fast & exact but not-always-linear algorithm
   DT_EGGERS_1998,       // Fast & exact but not-always-linear algorithm
   DT_IFT, DT_IFT_8,     // Falcao & Da Cunha algorithm (almost exact; fast)    
   DT_IFT_4,             // Same alg, w/4-neighborhood. You may call this alg. 
                         // with arbitrary neighborhood using its function directly.
   DT_EXACT_DILATIONS,   // Costa & Estrozi 1999 algorithm  (exact; slow)
   DT_BRUTE_FORCE,        // O(n^4) direct algorithm just for benchmarks
   DT_BRUTE_FORCE_WITH_LIST, // O(n^4) direct algorithm just for benchmarks
} dt_algorithm;

AnimalExport ImgPUInt32
   *distance_transform(Img *bin, dt_algorithm alg),
   *distance_transform_label(Img *bin, dt_algorithm alg, bool compute_label, ImgPUInt32 **imlabel),
   *distance_transform_max_dist(Img *bin, dt_algorithm alg, puint32 max_dist, 
       bool compute_label, ImgPUInt32 **imlabel);


AnimalExport bool
   distance_transform_ip(ImgPUInt32 *cost, dt_algorithm alg),
   distance_transform_ip_max_dist(ImgPUInt32 *cost, dt_algorithm alg, puint32 max_dist, 
       bool compute_label, ImgPUInt32 *imlabel),
   edt_lz(ImgPUInt32 *im),
   edt_lz_old(ImgPUInt32 *im),  /* more readable code but slower */
   edt_maurer2003(ImgPUInt32 *im),
   edt_maurer2003_label(ImgPUInt32 *im, ImgPUInt32 *imlabel),
   edt_meijster2000(ImgPUInt32 *im),
   edt_ift(ImgPUInt32 *im, nhood *adj),
   edt_saito(ImgPUInt32 *im),
   edt_eggers(ImgPUInt32 *im),
   edt_exact_dilations(ImgPUInt32 *im),
   edt_cuisenaire_psn4(ImgPUInt32 *im),  // simple 4-neighborhood approx. EDT
   edt_cuisenaire_psn8(ImgPUInt32 *im),  // simple 8-nhood approx. EDT
   edt_cuisenaire_pmn (ImgPUInt32 *im),  // multiple nhood, exact EDT
   edt_cuisenaire_pmn_max_dist(ImgPUInt32 *im, puint32 max_dist), // multiple nhood up to a maximum distance
   edt_cuisenaire_pmn_orig(ImgPUInt32 *im), // exactly the code provided by Cuisenaire
   edt_cuisenaire_pmon(ImgPUInt32 *im),  // multiple oriented nhood,
   edt_cuisenaire_pmon_max_dist(ImgPUInt32 *im, puint32 max_dist), // multiple oriented nhood up to a maximum distance
   edt_brute_force(ImgPUInt32 *im),     // O(n^4) algorithm just for benchmarks
   edt_brute_force_with_list(ImgPUInt32 *im), // O(n^2)-O(n^4) algorithm
   edt_lz_step1_vertical(ImgPUInt32 *im), // 1D columwise EDT
   edt_lz_step1_vertical_label(ImgPUInt32 *im, ImgPUInt32 *imlabel), // 1D columwise EDT with nearest pixel labels
   smallest_error_location(// generates a table of numbers for Cuisenaire.
         nhood *N, 
         puint32 *Derr_p, 
         puint32 *derr_x, puint32 *derr_y,
         puint32 start_dpx, puint32 start_dpy),
   closest_non_propagating_pixel (
         puint32 n, 
         puint32 *Dnp, puint32 *Derr, 
         puint32 *npx_p, puint32 *npy_p,
         puint32 *derr_x, puint32 *derr_y, 
         puint32 start_dpx, puint32 start_dpy); 

#define edt_1d_vertical(im) edt_lz_step1_vertical(im)
#define edt_1d_vertical_label(im, imlabel) edt_lz_step1_vertical_label(im, imlabel)

END_C_DECLS

#endif /* !DT_H */
