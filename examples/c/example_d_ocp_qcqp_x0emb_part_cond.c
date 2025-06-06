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

#include <stdlib.h>
#include <stdio.h>

#include <blasfeo_d_aux_ext_dep.h>

#include <hpipm_d_ocp_qcqp_ipm.h>
#include <hpipm_d_ocp_qcqp_dim.h>
#include <hpipm_d_ocp_qcqp.h>
#include <hpipm_d_ocp_qcqp_sol.h>
#include <hpipm_d_ocp_qcqp_red.h>
#include <hpipm_d_ocp_qcqp_utils.h>
#include <hpipm_d_part_cond_qcqp.h>
#include <hpipm_timing.h>



// qp data as global data
extern int N;
extern int *nx;
extern int *nu;
extern int *nbu;
extern int *nbx;
extern int *ng;
extern int *nq;
extern int *nsbx;
extern int *nsbu;
extern int *nsg;
extern int *nsq;
extern int *nbxe;
extern int *nbue;
extern int *nge;
extern int *nqe;
extern double **hA;
extern double **hB;
extern double **hb;
extern double **hQ;
extern double **hR;
extern double **hS;
extern double **hq;
extern double **hr;
extern int **hidxbx;
extern double **hlbx;
extern double **hlbx_mask;
extern double **hubx;
extern double **hubx_mask;
extern int **hidxbu;
extern double **hlbu;
extern double **hlbu_mask;
extern double **hubu;
extern double **hubu_mask;
extern double **hC;
extern double **hD;
extern double **hlg;
extern double **hlg_mask;
extern double **hug;
extern double **hug_mask;
extern double **hQq;
extern double **hRq;
extern double **hSq;
extern double **hqq;
extern double **hrq;
extern double **huq;
extern double **huq_mask;
extern double **hZl;
extern double **hZu;
extern double **hzl;
extern double **hzu;
extern int **hidxs;
extern double **hlls;
extern double **hlls_mask;
extern double **hlus;
extern double **hlus_mask;
extern int **hidxe;
// arg
extern int mode;
extern int iter_max;
extern double alpha_min;
extern double mu0;
extern double tol_stat;
extern double tol_eq;
extern double tol_ineq;
extern double tol_comp;
extern double reg_prim;
extern int warm_start;
extern int pred_corr;
extern int ric_alg;
extern int split_step;



// main
int main()
	{

	int ii;

	int hpipm_status;

	int rep, nrep=10;

	hpipm_timer timer;

/************************************************
* ocp qp dim
************************************************/

	hpipm_size_t dim_size = d_ocp_qcqp_dim_memsize(N);
	void *dim_mem = malloc(dim_size);

	struct d_ocp_qcqp_dim dim;
	d_ocp_qcqp_dim_create(N, &dim, dim_mem);

	for(ii=0; ii<=N; ii++)
		{
		d_ocp_qcqp_dim_set_nx(ii, nx[ii], &dim);
		d_ocp_qcqp_dim_set_nu(ii, nu[ii], &dim);
		d_ocp_qcqp_dim_set_nbx(ii, nbx[ii], &dim);
		d_ocp_qcqp_dim_set_nbu(ii, nbu[ii], &dim);
		d_ocp_qcqp_dim_set_ng(ii, ng[ii], &dim);
		d_ocp_qcqp_dim_set_nq(ii, nq[ii], &dim);
		d_ocp_qcqp_dim_set_nsbx(ii, nsbx[ii], &dim);
		d_ocp_qcqp_dim_set_nsbu(ii, nsbu[ii], &dim);
		d_ocp_qcqp_dim_set_nsg(ii, nsg[ii], &dim);
		d_ocp_qcqp_dim_set_nsq(ii, nsq[ii], &dim);
		d_ocp_qcqp_dim_set_nbxe(ii, nbxe[ii], &dim);
		d_ocp_qcqp_dim_set_nbue(ii, nbue[ii], &dim);
		d_ocp_qcqp_dim_set_nge(ii, nge[ii], &dim);
		d_ocp_qcqp_dim_set_nqe(ii, nqe[ii], &dim);
		}

//	d_ocp_qcqp_dim_print(&dim);
//	d_ocp_qp_dim_print(dim.qp_dim);
//	d_ocp_qcqp_dim_codegen("tmp.c", "w", &dim);

/************************************************
* ocp qp dim red eq dof (reduce equation dof, i.e. x0 elimination)
************************************************/

	hpipm_size_t dim_size3 = d_ocp_qcqp_dim_memsize(N);
	void *dim_mem3 = malloc(dim_size3);

	struct d_ocp_qcqp_dim dim3;
	d_ocp_qcqp_dim_create(N, &dim3, dim_mem3);

	d_ocp_qcqp_dim_reduce_eq_dof(&dim, &dim3);

//	d_ocp_qcqp_dim_print(&dim3);
//	d_ocp_qp_dim_print(dim3.qp_dim);
//	d_ocp_qcqp_dim_codegen("tmp3.c", "w", &dim3);

/************************************************
* ocp qp dim part cond
************************************************/

	// horizon length of partially condensed OCP QP
	int N2 = N/2;
	N2 = N2<1 ? 1 : N2;

	hpipm_size_t dim_size2 = d_ocp_qcqp_dim_memsize(N2);
	void *dim_mem2 = malloc(dim_size2);

	struct d_ocp_qcqp_dim dim2;
	d_ocp_qcqp_dim_create(N2, &dim2, dim_mem2);

	int *block_size = malloc((N+1)*sizeof(int));
	d_part_cond_qcqp_compute_block_size(N, N2, block_size);
//	block_size[0] = 1;
//	block_size[1] = 1;
//	printf("\nblock_size\n");
//	int_print_mat(1, N2+1, block_size, 1);

	d_part_cond_qcqp_compute_dim(&dim3, block_size, &dim2);

//	d_ocp_qcqp_dim_print(&dim2);
//	d_ocp_qp_dim_print(dim2.qp_dim);
//	d_ocp_qcqp_dim_codegen("tmp2.c", "w", &dim2);

/************************************************
* ocp qp
************************************************/

	hpipm_size_t qp_size = d_ocp_qcqp_memsize(&dim);
	void *qp_mem = malloc(qp_size);

	struct d_ocp_qcqp qp;
	d_ocp_qcqp_create(&dim, &qp, qp_mem);

	// dynamics
	ii = 0;
	for(ii=0; ii<N; ii++)
		{
		d_ocp_qcqp_set_A(ii, hA[ii], &qp);
		d_ocp_qcqp_set_B(ii, hB[ii], &qp);
		d_ocp_qcqp_set_b(ii, hb[ii], &qp);
		}

	// cost
	ii = 0;
	for(ii=0; ii<=N; ii++)
		{
		d_ocp_qcqp_set_Q(ii, hQ[ii], &qp);
		d_ocp_qcqp_set_S(ii, hS[ii], &qp);
		d_ocp_qcqp_set_R(ii, hR[ii], &qp);
		d_ocp_qcqp_set_q(ii, hq[ii], &qp);
		d_ocp_qcqp_set_r(ii, hr[ii], &qp);
		d_ocp_qcqp_set_Zl(ii, hZl[ii], &qp);
		d_ocp_qcqp_set_Zu(ii, hZu[ii], &qp);
		d_ocp_qcqp_set_zl(ii, hzl[ii], &qp);
		d_ocp_qcqp_set_zu(ii, hzu[ii], &qp);
		}
	
	// constraints
	ii = 0;
	for(ii=0; ii<=N; ii++)
		{
		d_ocp_qcqp_set_idxbx(ii, hidxbx[ii], &qp);
		d_ocp_qcqp_set_lbx(ii, hlbx[ii], &qp);
		d_ocp_qcqp_set_lbx_mask(ii, hlbx_mask[ii], &qp);
		d_ocp_qcqp_set_ubx(ii, hubx[ii], &qp);
		d_ocp_qcqp_set_ubx_mask(ii, hubx_mask[ii], &qp);
		d_ocp_qcqp_set_idxbu(ii, hidxbu[ii], &qp);
		d_ocp_qcqp_set_lbu(ii, hlbu[ii], &qp);
		d_ocp_qcqp_set_lbu_mask(ii, hlbu_mask[ii], &qp);
		d_ocp_qcqp_set_ubu(ii, hubu[ii], &qp);
		d_ocp_qcqp_set_ubu_mask(ii, hubu_mask[ii], &qp);
		d_ocp_qcqp_set_C(ii, hC[ii], &qp);
		d_ocp_qcqp_set_D(ii, hD[ii], &qp);
		d_ocp_qcqp_set_lg(ii, hlg[ii], &qp);
		d_ocp_qcqp_set_lg_mask(ii, hlg_mask[ii], &qp);
		d_ocp_qcqp_set_ug(ii, hug[ii], &qp);
		d_ocp_qcqp_set_ug_mask(ii, hug_mask[ii], &qp);
		d_ocp_qcqp_set_Rq(ii, hRq[ii], &qp);
		d_ocp_qcqp_set_Sq(ii, hSq[ii], &qp);
		d_ocp_qcqp_set_Qq(ii, hQq[ii], &qp);
		d_ocp_qcqp_set_rq(ii, hrq[ii], &qp);
		d_ocp_qcqp_set_qq(ii, hqq[ii], &qp);
		d_ocp_qcqp_set_uq(ii, huq[ii], &qp);
		d_ocp_qcqp_set_uq_mask(ii, huq_mask[ii], &qp);
		d_ocp_qcqp_set_idxs(ii, hidxs[ii], &qp);
		d_ocp_qcqp_set_lls(ii, hlls[ii], &qp);
		d_ocp_qcqp_set_lls_mask(ii, hlls_mask[ii], &qp);
		d_ocp_qcqp_set_lus(ii, hlus[ii], &qp);
		d_ocp_qcqp_set_lus_mask(ii, hlus_mask[ii], &qp);
		d_ocp_qcqp_set_idxe(ii, hidxe[ii], &qp);
		}
	
	d_ocp_qcqp_print(&dim, &qp);
//	d_ocp_qcqp_codegen("tmp.c", "a", &dim, &qp);

/************************************************
* ocp qp red eq dof
************************************************/

	hpipm_size_t qp_size3 = d_ocp_qcqp_memsize(&dim3);
	void *qp_mem3 = malloc(qp_size3);

	struct d_ocp_qcqp qp3;
	d_ocp_qcqp_create(&dim3, &qp3, qp_mem3);

/************************************************
* ocp qp part cond
************************************************/

	hpipm_size_t qp_size2 = d_ocp_qcqp_memsize(&dim2);
	void *qp_mem2 = malloc(qp_size2);

	struct d_ocp_qcqp qp2;
	d_ocp_qcqp_create(&dim2, &qp2, qp_mem2);

/************************************************
* ocp qp sol
************************************************/

	hpipm_size_t qp_sol_size = d_ocp_qcqp_sol_memsize(&dim);
	void *qp_sol_mem = malloc(qp_sol_size);

	struct d_ocp_qcqp_sol qp_sol;
	d_ocp_qcqp_sol_create(&dim, &qp_sol, qp_sol_mem);

/************************************************
* ocp qp sol part cond
************************************************/

	hpipm_size_t qp_sol_size2 = d_ocp_qcqp_sol_memsize(&dim2);
	void *qp_sol_mem2 = malloc(qp_sol_size2);

	struct d_ocp_qcqp_sol qp_sol2;
	d_ocp_qcqp_sol_create(&dim2, &qp_sol2, qp_sol_mem2);

/************************************************
* ocp qp sol red eq dof
************************************************/

	hpipm_size_t qp_sol_size3 = d_ocp_qcqp_sol_memsize(&dim3);
	void *qp_sol_mem3 = malloc(qp_sol_size3);

	struct d_ocp_qcqp_sol qp_sol3;
	d_ocp_qcqp_sol_create(&dim3, &qp_sol3, qp_sol_mem3);

/************************************************
* red eq dof arg
************************************************/

	hpipm_size_t qp_red_arg_size = d_ocp_qcqp_reduce_eq_dof_arg_memsize();
	void *qp_red_arg_mem = malloc(qp_red_arg_size);

	struct d_ocp_qcqp_reduce_eq_dof_arg qp_red_arg;
	d_ocp_qcqp_reduce_eq_dof_arg_create(&qp_red_arg, qp_red_arg_mem);

	int tmp_i1 = 1;
	d_ocp_qcqp_reduce_eq_dof_arg_set_default(&qp_red_arg);
	d_ocp_qcqp_reduce_eq_dof_arg_set_alias_unchanged(&tmp_i1, &qp_red_arg);
	d_ocp_qcqp_reduce_eq_dof_arg_set_comp_dual_sol_eq(&tmp_i1, &qp_red_arg);
	d_ocp_qcqp_reduce_eq_dof_arg_set_comp_dual_sol_ineq(&tmp_i1, &qp_red_arg);

/************************************************
* part cond arg
************************************************/

	hpipm_size_t part_cond_arg_size = d_part_cond_qcqp_arg_memsize(dim2.N);
	void *part_cond_arg_mem = malloc(part_cond_arg_size);

	struct d_part_cond_qcqp_arg part_cond_arg;
	d_part_cond_qcqp_arg_create(dim2.N, &part_cond_arg, part_cond_arg_mem);

	d_part_cond_qcqp_arg_set_default(&part_cond_arg);

//	d_part_cond_qcqp_arg_set_ric_alg(0, &part_cond_arg);

/************************************************
* ipm arg
************************************************/

	hpipm_size_t ipm_arg_size = d_ocp_qcqp_ipm_arg_memsize(&dim2);
	void *ipm_arg_mem = malloc(ipm_arg_size);

	struct d_ocp_qcqp_ipm_arg arg;
	d_ocp_qcqp_ipm_arg_create(&dim2, &arg, ipm_arg_mem);

	d_ocp_qcqp_ipm_arg_set_default(mode, &arg);

	d_ocp_qcqp_ipm_arg_set_mu0(&mu0, &arg);
	d_ocp_qcqp_ipm_arg_set_iter_max(&iter_max, &arg);
	d_ocp_qcqp_ipm_arg_set_tol_stat(&tol_stat, &arg);
	d_ocp_qcqp_ipm_arg_set_tol_eq(&tol_eq, &arg);
	d_ocp_qcqp_ipm_arg_set_tol_ineq(&tol_ineq, &arg);
	d_ocp_qcqp_ipm_arg_set_tol_comp(&tol_comp, &arg);
	d_ocp_qcqp_ipm_arg_set_reg_prim(&reg_prim, &arg);
	d_ocp_qcqp_ipm_arg_set_warm_start(&warm_start, &arg);
	d_ocp_qcqp_ipm_arg_set_pred_corr(&pred_corr, &arg);
	d_ocp_qcqp_ipm_arg_set_ric_alg(&ric_alg, &arg);
	d_ocp_qcqp_ipm_arg_set_split_step(&split_step, &arg);

	//double tau_min = 1e-2;
	//d_ocp_qcqp_ipm_arg_set_tau_min(&tau_min, &arg);

//	double mu0_tmp = 1e0;
//	d_ocp_qcqp_ipm_arg_set_mu0(&mu0_tmp, &arg);

//	double tol_tmp = 1e-3;
//	d_ocp_qcqp_ipm_arg_set_tol_stat(&tol_tmp, &arg);
//	d_ocp_qcqp_ipm_arg_set_tol_eq(&tol_tmp, &arg);
//	d_ocp_qcqp_ipm_arg_set_tol_ineq(&tol_tmp, &arg);
//	d_ocp_qcqp_ipm_arg_set_tol_comp(&tol_tmp, &arg);

//	int iter_max_tmp = 15;
//	d_ocp_qcqp_ipm_arg_set_iter_max(&iter_max_tmp, &arg);

/************************************************
* red eq dof workspace
************************************************/

	hpipm_size_t qp_red_work_size = d_ocp_qcqp_reduce_eq_dof_ws_memsize(&dim);
	void *qp_red_work_mem = malloc(qp_red_work_size);

	struct d_ocp_qcqp_reduce_eq_dof_ws qp_red_work;
	d_ocp_qcqp_reduce_eq_dof_ws_create(&dim, &qp_red_work, qp_red_work_mem);

/************************************************
* part cond workspace
************************************************/

	hpipm_size_t part_cond_size = d_part_cond_qcqp_ws_memsize(&dim3, block_size, &dim2, &part_cond_arg);
	void *part_cond_mem = malloc(part_cond_size);

	struct d_part_cond_qcqp_ws part_cond_ws;
	d_part_cond_qcqp_ws_create(&dim3, block_size, &dim2, &part_cond_arg, &part_cond_ws, part_cond_mem);

/************************************************
* ipm workspace
************************************************/

	hpipm_size_t ipm_size = d_ocp_qcqp_ipm_ws_memsize(&dim2, &arg);
	void *ipm_mem = malloc(ipm_size);

	struct d_ocp_qcqp_ipm_ws workspace;
	d_ocp_qcqp_ipm_ws_create(&dim2, &arg, &workspace, ipm_mem);

/************************************************
* reduce equation dof (i.e. x0 elimination)
************************************************/

	hpipm_tic(&timer);

	for(rep=0; rep<nrep; rep++)
		{
		d_ocp_qcqp_reduce_eq_dof(&qp, &qp3, &qp_red_arg, &qp_red_work);
		}

	double time_red_eq_dof = hpipm_toc(&timer) / nrep;

	d_ocp_qcqp_print(&dim3, &qp3);
//	d_ocp_qcqp_codegen("tmp3.c", "a", &dim3, &qp3);

/************************************************
* part cond
************************************************/

	hpipm_tic(&timer);

	for(rep=0; rep<nrep; rep++)
		{
		d_part_cond_qcqp_cond(&qp3, &qp2, &part_cond_arg, &part_cond_ws);
		}

	double time_cond = hpipm_toc(&timer) / nrep;

	d_ocp_qcqp_print(&dim2, &qp2);
//	d_ocp_qcqp_codegen("tmp2.c", "a", &dim2, &qp2);
//printf("\ncaxxo\n");
//exit(1);

/************************************************
* ipm solver
************************************************/

	hpipm_tic(&timer);

	for(rep=0; rep<nrep; rep++)
		{
		d_ocp_qcqp_ipm_solve(&qp2, &qp_sol2, &arg, &workspace);
		d_ocp_qcqp_ipm_get_status(&workspace, &hpipm_status);
		}

	double time_ipm = hpipm_toc(&timer) / nrep;

	d_ocp_qcqp_sol_print(&dim2, &qp_sol2);

/************************************************
* part expand
************************************************/

	hpipm_tic(&timer);

	for(rep=0; rep<nrep; rep++)
		{
		d_part_cond_qcqp_expand_sol(&qp3, &qp_sol2, &qp_sol3, &part_cond_arg, &part_cond_ws);
		}

	double time_expa = hpipm_toc(&timer) / nrep;

	d_ocp_qcqp_sol_print(&dim3, &qp_sol3);

/************************************************
* restore equation dof
************************************************/

	hpipm_tic(&timer);

	for(rep=0; rep<nrep; rep++)
		{
		d_ocp_qcqp_restore_eq_dof(&qp, &qp_sol3, &qp_sol, &qp_red_arg, &qp_red_work);
		}

	double time_res_eq_dof = hpipm_toc(&timer) / nrep;

	d_ocp_qcqp_sol_print(&dim, &qp_sol);

/************************************************
* print solution info
************************************************/

    printf("\nHPIPM returned with flag %i.\n", hpipm_status);
    if(hpipm_status == 0)
		{
        printf("\n -> QP solved!\n");
		}
	else if(hpipm_status==1)
		{
        printf("\n -> Solver failed! Maximum number of iterations reached\n");
		}
	else if(hpipm_status==2)
		{
        printf("\n -> Solver failed! Minimum step lenght reached\n");
		}
	else if(hpipm_status==2)
		{
        printf("\n -> Solver failed! NaN in computations\n");
		}
	else
		{
        printf("\n -> Solver failed! Unknown return flag\n");
		}
    printf("\nAverage solution time over %i runs: %e [s]\n", nrep, time_ipm);
	printf("\n\n");

/************************************************
* extract and print solution
************************************************/

	// u

	int nu_max = nu[0];
	for(ii=1; ii<=N; ii++)
		if(nu[ii]>nu_max)
			nu_max = nu[ii];

	double *u = malloc(nu_max*sizeof(double));

	printf("\nu = \n");
	for(ii=0; ii<=N; ii++)
		{
		d_ocp_qcqp_sol_get_u(ii, &qp_sol, u);
		d_print_mat(1, nu[ii], u, 1);
		}

	// x

	int nx_max = nx[0];
	for(ii=1; ii<=N; ii++)
		if(nx[ii]>nx_max)
			nx_max = nx[ii];

	double *x = malloc(nx_max*sizeof(double));

	printf("\nx = \n");
	for(ii=0; ii<=N; ii++)
		{
		d_ocp_qcqp_sol_get_x(ii, &qp_sol, x);
		d_print_mat(1, nx[ii], x, 1);
		}
	
//	d_ocp_qcqp_print(&dim, &qp);
//	d_ocp_qcqp_sol_print(&dim, &qp_sol);

/************************************************
* print ipm statistics
************************************************/

	int iter; d_ocp_qcqp_ipm_get_iter(&workspace, &iter);
	double res_stat; d_ocp_qcqp_ipm_get_max_res_stat(&workspace, &res_stat);
	double res_eq; d_ocp_qcqp_ipm_get_max_res_eq(&workspace, &res_eq);
	double res_ineq; d_ocp_qcqp_ipm_get_max_res_ineq(&workspace, &res_ineq);
	double res_comp; d_ocp_qcqp_ipm_get_max_res_comp(&workspace, &res_comp);
	double *stat; d_ocp_qcqp_ipm_get_stat(&workspace, &stat);
	int stat_m; d_ocp_qcqp_ipm_get_stat_m(&workspace, &stat_m);

	printf("\nipm return = %d\n", hpipm_status);
	printf("\nipm residuals max: res_g = %e, res_b = %e, res_d = %e, res_m = %e\n", res_stat, res_eq, res_ineq, res_comp);

	printf("\nipm iter = %d\n", iter);
	printf("\nalpha_aff\tmu_aff\t\tsigma\t\talpha_prim\talpha_dual\tmu\t\tres_stat\tres_eq\t\tres_ineq\tres_comp\tdual_gap\tobj\t\tlq fact\t\titref pred\titref corr\tlin res stat\tlin res eq\tlin res ineq\tlin res comp\n");
	d_print_exp_tran_mat(stat_m, iter+1, stat, stat_m);

	printf("\nred eq for time = %e [s]\n\n", time_red_eq_dof);
	printf("\npart cond time = %e [s]\n\n", time_cond);
	printf("\nocp ipm time = %e [s]\n\n", time_ipm);
	printf("\npart expa time = %e [s]\n\n", time_expa);
	printf("\nres eq for time = %e [s]\n\n", time_res_eq_dof);
	printf("\ntotal time = %e [s]\n\n", time_red_eq_dof+time_cond+time_ipm+time_expa+time_res_eq_dof);

/************************************************
* free memory and return
************************************************/

    free(dim_mem);
    free(dim_mem2);
    free(dim_mem3);
    free(block_size);
    free(qp_mem);
    free(qp_mem2);
    free(qp_mem3);
	free(qp_sol_mem);
	free(qp_sol_mem2);
	free(qp_red_arg_mem);
	free(part_cond_arg_mem);
	free(ipm_arg_mem);
	free(part_cond_mem);
	free(ipm_mem);

	free(u);
	free(x);

	return 0;

	}




