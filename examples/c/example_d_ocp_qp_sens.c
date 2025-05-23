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

#include <blasfeo_common.h>
#include <blasfeo_d_aux_ext_dep.h>

#include <hpipm_d_ocp_qp_ipm.h>
#include <hpipm_d_ocp_qp_dim.h>
#include <hpipm_d_ocp_qp.h>
#include <hpipm_d_ocp_qp_sol.h>
#include <hpipm_d_ocp_qp_seed.h>
#include <hpipm_d_ocp_qp_utils.h>
#include <hpipm_timing.h>



// qp data as global data
extern int N;
extern int *nx;
extern int *nu;
extern int *nbu;
extern int *nbx;
extern int *ng;
extern int *nsbx;
extern int *nsbu;
extern int *nsg;
extern int *nbue;
extern int *nbxe;
extern int *nge;
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
//extern double **hu_guess;
//extern double **hx_guess;
//extern double **hsl_guess;
//extern double **hsu_guess;
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



// main
int main()
	{

	int ii, jj;

	int hpipm_status;

	int rep, nrep=10;

	hpipm_timer timer;

/************************************************
* ocp qp dim
************************************************/

	hpipm_size_t dim_size = d_ocp_qp_dim_memsize(N);
	void *dim_mem = malloc(dim_size);

	struct d_ocp_qp_dim dim;
	d_ocp_qp_dim_create(N, &dim, dim_mem);

	// unified setter
	d_ocp_qp_dim_set_all(nx, nu, nbx, nbu, ng, nsbx, nsbu, nsg, &dim);

	// additional single setters

	// set number of inequality constr to be considered as equality constr
	// (ignored in this example)
	//for(ii=0; ii<=N; ii++)
	//	{
	//	d_ocp_qp_dim_set_nbxe(ii, nbxe[ii], &dim);
	//	d_ocp_qp_dim_set_nbue(ii, nbue[ii], &dim);
	//	d_ocp_qp_dim_set_nge(ii, nge[ii], &dim);
	//	}

	//d_ocp_qp_dim_codegen("examples/c/data/test_d_ocp_data.c", "w", &dim);

/************************************************
* ocp qp
************************************************/

	hpipm_size_t qp_size = d_ocp_qp_memsize(&dim);
	void *qp_mem = malloc(qp_size);

	struct d_ocp_qp qp;
	d_ocp_qp_create(&dim, &qp, qp_mem);

	// unified setter
	d_ocp_qp_set_all(hA, hB, hb, hQ, hS, hR, hq, hr, hidxbx, hlbx, hubx, hidxbu, hlbu, hubu, hC, hD, hlg, hug, hZl, hZu, hzl, hzu, hidxs, hlls, hlus, &qp);

	// additional single setters

	// mark the inequality constr to be considered as equality constr
	// (ignored in this example)
	//for(ii=0; ii<=N; ii++)
	//	{
	//	d_ocp_qp_set_idxe(ii, hidxe[ii], &qp);
	//	}

	// set inequality constraints mask
	for(ii=0; ii<=N; ii++)
		{
		d_ocp_qp_set_lbu_mask(ii, hlbu_mask[ii], &qp);
		d_ocp_qp_set_ubu_mask(ii, hubu_mask[ii], &qp);
		d_ocp_qp_set_lbx_mask(ii, hlbx_mask[ii], &qp);
		d_ocp_qp_set_ubx_mask(ii, hubx_mask[ii], &qp);
		d_ocp_qp_set_lg_mask(ii, hlg_mask[ii], &qp);
		d_ocp_qp_set_ug_mask(ii, hug_mask[ii], &qp);
		d_ocp_qp_set_lls_mask(ii, hlls_mask[ii], &qp);
		d_ocp_qp_set_lus_mask(ii, hlus_mask[ii], &qp);
		}

/************************************************
* ocp qp sol
************************************************/

	hpipm_size_t qp_sol_size = d_ocp_qp_sol_memsize(&dim);
	void *qp_sol_mem = malloc(qp_sol_size);

	struct d_ocp_qp_sol qp_sol;
	d_ocp_qp_sol_create(&dim, &qp_sol, qp_sol_mem);

/************************************************
* ipm arg
************************************************/

	hpipm_size_t ipm_arg_size = d_ocp_qp_ipm_arg_memsize(&dim);
	void *ipm_arg_mem = malloc(ipm_arg_size);

	struct d_ocp_qp_ipm_arg arg;
	d_ocp_qp_ipm_arg_create(&dim, &arg, ipm_arg_mem);

	d_ocp_qp_ipm_arg_set_default(mode, &arg);

	d_ocp_qp_ipm_arg_set_mu0(&mu0, &arg);
	d_ocp_qp_ipm_arg_set_iter_max(&iter_max, &arg);
	d_ocp_qp_ipm_arg_set_tol_stat(&tol_stat, &arg);
	d_ocp_qp_ipm_arg_set_tol_eq(&tol_eq, &arg);
	d_ocp_qp_ipm_arg_set_tol_ineq(&tol_ineq, &arg);
	d_ocp_qp_ipm_arg_set_tol_comp(&tol_comp, &arg);
	d_ocp_qp_ipm_arg_set_reg_prim(&reg_prim, &arg);
	d_ocp_qp_ipm_arg_set_warm_start(&warm_start, &arg);
//	d_ocp_qp_ipm_arg_set_ric_alg(&ric_alg, &arg);

/************************************************
* ipm workspace
************************************************/

	hpipm_size_t ipm_size = d_ocp_qp_ipm_ws_memsize(&dim, &arg);
	void *ipm_mem = malloc(ipm_size);

	struct d_ocp_qp_ipm_ws workspace;
	d_ocp_qp_ipm_ws_create(&dim, &arg, &workspace, ipm_mem);

/************************************************
* ipm solver
************************************************/

	hpipm_tic(&timer);

	for(rep=0; rep<nrep; rep++)
		{
		// solution guess
//		for(ii=0; ii<=N; ii++)
//			d_cvt_colmaj_to_ocp_qp_sol_u(ii, hu_guess[ii], &qp_sol);
//		for(ii=0; ii<=N; ii++)
//			d_cvt_colmaj_to_ocp_qp_sol_x(ii, hx_guess[ii], &qp_sol);
//		for(ii=0; ii<=N; ii++)
//			d_cvt_colmaj_to_ocp_qp_sol_sl(ii, hsl_guess[ii], &qp_sol);
//		for(ii=0; ii<=N; ii++)
//			d_cvt_colmaj_to_ocp_qp_sol_su(ii, hsu_guess[ii], &qp_sol);

		// call solver
		d_ocp_qp_ipm_solve(&qp, &qp_sol, &arg, &workspace);
		d_ocp_qp_ipm_get_status(&workspace, &hpipm_status);
		}

	double time_ipm = hpipm_toc(&timer) / nrep;

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
		d_ocp_qp_sol_get_u(ii, &qp_sol, u);
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
		d_ocp_qp_sol_get_x(ii, &qp_sol, x);
		d_print_mat(1, nx[ii], x, 1);
		}

	// pi
	double *pi = malloc(nx_max*sizeof(double));

	printf("\npi = \n");
	for(ii=0; ii<N; ii++)
		{
		d_ocp_qp_sol_get_pi(ii, &qp_sol, pi);
		d_print_mat(1, nx[ii+1], pi, 1);
		}

/************************************************
* print ipm statistics
************************************************/

	int iter; d_ocp_qp_ipm_get_iter(&workspace, &iter);
	double res_stat; d_ocp_qp_ipm_get_max_res_stat(&workspace, &res_stat);
	double res_eq; d_ocp_qp_ipm_get_max_res_eq(&workspace, &res_eq);
	double res_ineq; d_ocp_qp_ipm_get_max_res_ineq(&workspace, &res_ineq);
	double res_comp; d_ocp_qp_ipm_get_max_res_comp(&workspace, &res_comp);
	double *stat; d_ocp_qp_ipm_get_stat(&workspace, &stat);
	int stat_m; d_ocp_qp_ipm_get_stat_m(&workspace, &stat_m);

	printf("\nipm return = %d\n", hpipm_status);
	printf("\nipm residuals max: res_g = %e, res_b = %e, res_d = %e, res_m = %e\n", res_stat, res_eq, res_ineq, res_comp);

	printf("\nipm iter = %d\n", iter);
	printf("\nalpha_aff\tmu_aff\t\tsigma\t\talpha_prim\talpha_dual\tmu\t\tres_stat\tres_eq\t\tres_ineq\tres_comp\tdual_gap\tobj\t\tlq fact\t\titref pred\titref corr\tlin res stat\tlin res eq\tlin res ineq\tlin res comp\n");
	d_print_exp_tran_mat(stat_m, iter+1, stat, stat_m);

	printf("\nocp ipm time = %e [s]\n\n", time_ipm);

/************************************************
* predict solution of QP with new RHS
************************************************/

	void *qp1_mem = malloc(qp_size);
	struct d_ocp_qp qp1;
	d_ocp_qp_create(&dim, &qp1, qp1_mem);

	void *qp_sol1_mem = malloc(qp_sol_size);
	struct d_ocp_qp_sol qp_sol1;
	d_ocp_qp_sol_create(&dim, &qp_sol1, qp_sol1_mem);

	// slightly modify RHS of QP
	d_ocp_qp_copy_all(&qp, &qp1);

	double *lbx0_tmp = malloc(nx[0]*sizeof(double));
	double *ubx0_tmp = malloc(nx[0]*sizeof(double));
	//
	for(ii=0; ii<nx[0]; ii++)
		lbx0_tmp[ii] = hlbx[0][ii];
	for(ii=0; ii<nx[0]; ii++)
		ubx0_tmp[ii] = hubx[0][ii];
	lbx0_tmp[0] = 1.1*hlbx[0][0];
	ubx0_tmp[0] = 1.1*hubx[0][0];
	//
//	lbx0_tmp[1] = 0.95*hlbx[0][1];
//	ubx0_tmp[1] = 0.95*hubx[0][1];

	d_ocp_qp_set_lbx(0, lbx0_tmp, &qp1);
	d_ocp_qp_set_ubx(0, ubx0_tmp, &qp1);

	hpipm_tic(&timer);

	for(rep=0; rep<nrep; rep++)
		{
		d_ocp_qp_ipm_predict(&qp1, &qp_sol1, &arg, &workspace);
		}

	double time_pred = hpipm_toc(&timer) / nrep;

	printf("\nAverage prediction time over %i runs: %e [s]\n", nrep, time_pred);
	printf("\n\n");

	// predicted solution

	// u
	printf("\nu_pred = \n");
	for(ii=0; ii<=N; ii++)
		{
		d_ocp_qp_sol_get_u(ii, &qp_sol1, u);
		d_print_mat(1, nu[ii], u, 1);
		}

	// x
	printf("\nx_pred = \n");
	for(ii=0; ii<=N; ii++)
		{
		d_ocp_qp_sol_get_x(ii, &qp_sol1, x);
		d_print_mat(1, nx[ii], x, 1);
		}

	// pi
	printf("\npi_pred = \n");
	for(ii=0; ii<N; ii++)
		{
		d_ocp_qp_sol_get_pi(ii, &qp_sol1, pi);
		d_print_mat(1, nx[ii+1], pi, 1);
		}

	d_ocp_qp_ipm_get_max_res_stat(&workspace, &res_stat);
	d_ocp_qp_ipm_get_max_res_eq(&workspace, &res_eq);
	d_ocp_qp_ipm_get_max_res_ineq(&workspace, &res_ineq);
	d_ocp_qp_ipm_get_max_res_comp(&workspace, &res_comp);
	printf("\nprediction residuals max: res_g = %e, res_b = %e, res_d = %e, res_m = %e\n", res_stat, res_eq, res_ineq, res_comp);

/************************************************
* sensitivity of solution of QP
************************************************/

	// res struct
	hpipm_size_t seed_size = d_ocp_qp_seed_memsize(&dim);
	void *seed_mem = malloc(seed_size);
	struct d_ocp_qp_seed seed;
	d_ocp_qp_seed_create(&dim, &seed, seed_mem);

	// new sol struct
	void *sens_mem = malloc(qp_sol_size);
	struct d_ocp_qp_sol sens;
	d_ocp_qp_sol_create(&dim, &sens, sens_mem);

	// set seeds to zero
	d_ocp_qp_seed_set_zero(&seed);

	// set I to param
	double *seed_x0 = malloc(nx[0]*sizeof(double));
	for(ii=0; ii<nx[0]; ii++)
		seed_x0[ii] = 0.0;
	int index = 0;
	seed_x0[index] = 1.0;
	int stage = 0;
	d_ocp_qp_seed_set_seed_lbx(stage, seed_x0, &seed);
	d_ocp_qp_seed_set_seed_ubx(stage, seed_x0, &seed);

	// print seeds
	//d_ocp_qp_seed_print(seed.dim, &seed);

	// forward sensitivity of solution
	d_ocp_qp_ipm_sens_frw(&qp, &seed, &sens, &arg, &workspace);
	// adjoint sensitivity of solution
	//d_ocp_qp_ipm_sens_adj(&qp, &seed, &sens, &arg, &workspace);

	// u
	printf("\nu_sens = \n");
	for(ii=0; ii<=N; ii++)
		{
		d_ocp_qp_sol_get_u(ii, &sens, u);
		d_print_mat(1, nu[ii], u, 1);
		}

	// x
	printf("\nx_sens = \n");
	for(ii=0; ii<=N; ii++)
		{
		d_ocp_qp_sol_get_x(ii, &sens, x);
		d_print_mat(1, nx[ii], x, 1);
		}

	// pi
	printf("\npi_sens = \n");
	for(ii=0; ii<N; ii++)
		{
		d_ocp_qp_sol_get_pi(ii, &sens, pi);
		d_print_mat(1, nx[ii+1], pi, 1);
		}

	// print solution sensitivities
	//d_ocp_qp_sol_print(sens.dim, &sens);

/************************************************
* free memory and return
************************************************/

	free(dim_mem);
	free(qp_mem);
	free(qp1_mem);
	free(qp_sol_mem);
	free(qp_sol1_mem);
	free(seed_mem);
	free(sens_mem);
	free(seed_x0);
	free(ipm_arg_mem);
	free(ipm_mem);

	free(lbx0_tmp);
	free(ubx0_tmp);

	free(u);
	free(x);
	free(pi);

	return 0;

	}


