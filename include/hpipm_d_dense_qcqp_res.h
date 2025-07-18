/**************************************************************************************************
*                                                                                                 *
* This file is part of HPIPM.                                                                     *
*                                                                                                 *
* HPIPM -- High-Performance Interior Point Method.                                                *
* Copyright (C) 2019 by Gianluca Frison.                                                          *
* Developed at IMTEK (University of Freiburg) under the supervision of Moritz Diehl.              *
* All rights reserved.                                                                            *
*                                                                                                 *
* The 2-Clause BSD License                                                                        *
*                                                                                                 *
* Redistribution and use in source and binary forms, with or without                              *
* modification, are permitted provided that the following conditions are met:                     *
*                                                                                                 *
* 1. Redistributions of source code must retain the above copyright notice, this                  *
*    list of conditions and the following disclaimer.                                             *
* 2. Redistributions in binary form must reproduce the above copyright notice,                    *
*    this list of conditions and the following disclaimer in the documentation                    *
*    and/or other materials provided with the distribution.                                       *
*                                                                                                 *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND                 *
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED                   *
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE                          *
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR                 *
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES                  *
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;                    *
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND                     *
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT                      *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS                   *
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                                    *
*                                                                                                 *
* Author: Gianluca Frison, gianluca.frison (at) imtek.uni-freiburg.de                             *
*                                                                                                 *
**************************************************************************************************/

#ifndef HPIPM_D_DENSE_QCQP_RES_H_
#define HPIPM_D_DENSE_QCQP_RES_H_



#include <blasfeo_target.h>
#include <blasfeo_common.h>

#include <hpipm_d_dense_qcqp_dim.h>
#include <hpipm_d_dense_qcqp.h>
#include <hpipm_d_dense_qcqp_sol.h>



#ifdef __cplusplus
extern "C" {
#endif



struct d_dense_qcqp_res
	{
	struct d_dense_qcqp_dim *dim;
	struct blasfeo_dvec *res_g; // q-residuals
	struct blasfeo_dvec *res_b; // b-residuals
	struct blasfeo_dvec *res_d; // d-residuals
	struct blasfeo_dvec *res_m; // m-residuals
	double res_max[4]; // infinity norm of residuals
	double res_mu_sum; // sum of res_m
	double res_mu; // mu-residual = res_mu_sum / #constr
	double obj; // (primal) objective
	double dual_gap; // duality gap
	hpipm_size_t memsize;
	};



struct d_dense_qcqp_res_ws
	{
	struct blasfeo_dvec *tmp_nv; // work space of size nv
	struct blasfeo_dvec *tmp_nbgq; // work space of size nbM+ngM+nqM
	struct blasfeo_dvec *tmp_ns; // work space of size nsM
	struct blasfeo_dvec *tmp_lam_mask; // workspace of size 2*nbM+2*ngM+2*nsM
	struct blasfeo_dvec *q_fun; // value for evaluation of quadr constr
	struct blasfeo_dvec *q_adj; // value for adjoint of quadr constr
	double nc_mask_inv; // reciprocal of number of non-masked constraints
	int use_q_fun; // reuse cached value for evaluation of quadr constr
	int use_q_adj; // reuse cached value for adjoint of quadr constr
	int valid_nc_mask; // signal that there mask_constr and nc_mask_inv are valid
	int mask_constr; // signal that there are some masked constraints
	hpipm_size_t memsize;
	};



//
hpipm_size_t d_dense_qcqp_res_memsize(struct d_dense_qcqp_dim *dim);
//
void d_dense_qcqp_res_create(struct d_dense_qcqp_dim *dim, struct d_dense_qcqp_res *res, void *mem);
//
hpipm_size_t d_dense_qcqp_res_ws_memsize(struct d_dense_qcqp_dim *dim);
//
void d_dense_qcqp_res_ws_create(struct d_dense_qcqp_dim *dim, struct d_dense_qcqp_res_ws *workspace, void *mem);
//
void d_dense_qcqp_res_compute(struct d_dense_qcqp *qp, struct d_dense_qcqp_sol *qp_sol, struct d_dense_qcqp_res *res, struct d_dense_qcqp_res_ws *ws);
//
void d_dense_qcqp_res_compute_inf_norm(struct d_dense_qcqp_res *res);



#ifdef __cplusplus
}	// #extern "C"
#endif


#endif // HPIPM_D_DENSE_QCQP_RES_H_



