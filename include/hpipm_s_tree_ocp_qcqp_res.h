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

#ifndef HPIPM_S_TREE_OCP_QCQP_RES_H_
#define HPIPM_S_TREE_OCP_QCQP_RES_H_



#include <blasfeo_target.h>
#include <blasfeo_common.h>

#include <hpipm_common.h>
#include <hpipm_s_tree_ocp_qcqp_dim.h>
#include <hpipm_s_tree_ocp_qcqp.h>
#include <hpipm_s_tree_ocp_qcqp_sol.h>



#ifdef __cplusplus
extern "C" {
#endif



struct s_tree_ocp_qcqp_res
	{
	struct s_tree_ocp_qcqp_dim *dim;
	struct blasfeo_svec *res_g; // q-residuals
	struct blasfeo_svec *res_b; // b-residuals
	struct blasfeo_svec *res_d; // d-residuals
	struct blasfeo_svec *res_m; // m-residuals
	float res_max[4]; // max of residuals
	float res_mu_sum; // sum of res_m
	float res_mu; // mu-residual = res_mu_sum / #constr
	float obj; // (primal) objective
	float dual_gap; // duality gap
	hpipm_size_t memsize;
	};



struct s_tree_ocp_qcqp_res_ws
	{
	struct blasfeo_svec *tmp_nuxM; // work space of size nuM+nxM
	struct blasfeo_svec *tmp_nbgqM; // work space of size nbM+ngM+nqM
	struct blasfeo_svec *tmp_nsM; // work space of size nsM
	struct blasfeo_svec *tmp_lam_mask; // workspace of size 2*nbM+2*ngM+2*nsM
	struct blasfeo_svec *q_fun; // value for evaluation of quadr constr
	struct blasfeo_svec *q_adj; // value for adjoint of quadr constr
	float nc_mask_inv; // reciprocal of number of non-masked constraints
	int use_q_fun; // reuse cached value for evaluation of quadr constr
	int use_q_adj; // reuse cached value for adjoint of quadr constr
	int valid_nc_mask; // signal that there mask_constr and nc_mask_inv are valid
	int mask_constr; // signal that there are some masked constraints
	hpipm_size_t memsize;
	};



//
hpipm_size_t s_tree_ocp_qcqp_res_memsize(struct s_tree_ocp_qcqp_dim *ocp_dim);
//
void s_tree_ocp_qcqp_res_create(struct s_tree_ocp_qcqp_dim *ocp_dim, struct s_tree_ocp_qcqp_res *res, void *mem);
//
hpipm_size_t s_tree_ocp_qcqp_res_ws_memsize(struct s_tree_ocp_qcqp_dim *ocp_dim);
//
void s_tree_ocp_qcqp_res_ws_create(struct s_tree_ocp_qcqp_dim *ocp_dim, struct s_tree_ocp_qcqp_res_ws *ws, void *mem);
//
void s_tree_ocp_qcqp_res_compute(struct s_tree_ocp_qcqp *qp, struct s_tree_ocp_qcqp_sol *qp_sol, struct s_tree_ocp_qcqp_res *res, struct s_tree_ocp_qcqp_res_ws *ws);
//
void s_tree_ocp_qcqp_res_compute_inf_norm(struct s_tree_ocp_qcqp_res *res);



#ifdef __cplusplus
}	// #extern "C"
#endif


#endif // HPIPM_S_TREE_OCP_QCQP_RES_H_




