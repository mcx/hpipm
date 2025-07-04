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



hpipm_size_t OCP_QP_RES_MEMSIZE(struct OCP_QP_DIM *dim)
	{

	// loop index
	int ii;

	// extract ocp qp size
	int N = dim->N;
	int *nx = dim->nx;
	int *nu = dim->nu;
	int *nb = dim->nb;
	int *ng = dim->ng;
	int *ns = dim->ns;

	// compute core qp size
	int nvt = 0;
	int net = 0;
	int nct = 0;
	for(ii=0; ii<N; ii++)
		{
		nvt += nx[ii]+nu[ii]+2*ns[ii];
		net += nx[ii+1];
		nct += 2*nb[ii]+2*ng[ii]+2*ns[ii];
		}
	nvt += nx[ii]+nu[ii]+2*ns[ii];
	nct += 2*nb[ii]+2*ng[ii]+2*ns[ii];

	hpipm_size_t size = 0;

	size += 3*(N+1)*sizeof(struct STRVEC); // res_g res_d res_m
	size += 1*N*sizeof(struct STRVEC); // res_b

	size += 1*SIZE_STRVEC(nvt); // res_g
	size += 1*SIZE_STRVEC(net); // res_b
	size += 2*SIZE_STRVEC(nct); // res_d res_m

	size = (size+63)/64*64; // make multiple of typical cache line size
	size += 1*64; // align once to typical cache line size

	return size;

	}



void OCP_QP_RES_CREATE(struct OCP_QP_DIM *dim, struct OCP_QP_RES *res, void *mem)
	{

	// loop index
	int ii;

	// zero memory (to avoid corrupted memory like e.g. NaN)
	hpipm_size_t memsize = OCP_QP_RES_MEMSIZE(dim);
	hpipm_zero_memset(memsize, mem);

	// extract ocp qp size
	int N = dim->N;
	int *nx = dim->nx;
	int *nu = dim->nu;
	int *nb = dim->nb;
	int *ng = dim->ng;
	int *ns = dim->ns;

	// compute core qp size
	int nvt = 0;
	int net = 0;
	int nct = 0;
	for(ii=0; ii<N; ii++)
		{
		nvt += nx[ii]+nu[ii]+2*ns[ii];
		net += nx[ii+1];
		nct += 2*nb[ii]+2*ng[ii]+2*ns[ii];
		}
	nvt += nx[ii]+nu[ii]+2*ns[ii];
	nct += 2*nb[ii]+2*ng[ii]+2*ns[ii];


	// vector struct
	struct STRVEC *sv_ptr = (struct STRVEC *) mem;

	res->res_g = sv_ptr;
	sv_ptr += N+1;
	res->res_b = sv_ptr;
	sv_ptr += N;
	res->res_d = sv_ptr;
	sv_ptr += N+1;
	res->res_m = sv_ptr;
	sv_ptr += N+1;


	// align to typical cache line size
	hpipm_size_t s_ptr = (hpipm_size_t) sv_ptr;
	s_ptr = (s_ptr+63)/64*64;


	// void stuf
	char *c_ptr = (char *) s_ptr;

	CREATE_STRVEC(nvt, res->res_g, c_ptr);
	c_ptr += SIZE_STRVEC(nvt);

	CREATE_STRVEC(net, res->res_b, c_ptr);
	c_ptr += SIZE_STRVEC(net);

	CREATE_STRVEC(nct, res->res_d, c_ptr);
	c_ptr += SIZE_STRVEC(nct);

	CREATE_STRVEC(nct, res->res_m, c_ptr);
	c_ptr += SIZE_STRVEC(nct);

	// alias
	//
	c_ptr = (char *) res->res_g->pa;
	for(ii=0; ii<=N; ii++)
		{
		CREATE_STRVEC(nu[ii]+nx[ii]+2*ns[ii], res->res_g+ii, c_ptr);
		c_ptr += nu[ii]*sizeof(REAL);
		c_ptr += nx[ii]*sizeof(REAL);
		c_ptr += ns[ii]*sizeof(REAL);
		c_ptr += ns[ii]*sizeof(REAL);
		}
	//
	c_ptr = (char *) res->res_b->pa;
	for(ii=0; ii<N; ii++)
		{
		CREATE_STRVEC(nx[ii+1], res->res_b+ii, c_ptr);
		c_ptr += (nx[ii+1])*sizeof(REAL);
		}
	//
	c_ptr = (char *) res->res_d->pa;
	for(ii=0; ii<=N; ii++)
		{
		CREATE_STRVEC(2*nb[ii]+2*ng[ii]+2*ns[ii], res->res_d+ii, c_ptr);
		c_ptr += nb[ii]*sizeof(REAL);
		c_ptr += ng[ii]*sizeof(REAL);
		c_ptr += nb[ii]*sizeof(REAL);
		c_ptr += ng[ii]*sizeof(REAL);
		c_ptr += ns[ii]*sizeof(REAL);
		c_ptr += ns[ii]*sizeof(REAL);
		}
	//
	c_ptr = (char *) res->res_m->pa;
	for(ii=0; ii<=N; ii++)
		{
		CREATE_STRVEC(2*nb[ii]+2*ng[ii]+2*ns[ii], res->res_m+ii, c_ptr);
		c_ptr += nb[ii]*sizeof(REAL);
		c_ptr += ng[ii]*sizeof(REAL);
		c_ptr += nb[ii]*sizeof(REAL);
		c_ptr += ng[ii]*sizeof(REAL);
		c_ptr += ns[ii]*sizeof(REAL);
		c_ptr += ns[ii]*sizeof(REAL);
		}



	res->dim = dim;

	res->memsize = memsize; //OCP_QP_RES_MEMSIZE(dim);


#if defined(RUNTIME_CHECKS)
	if(c_ptr > ((char *) mem) + res->memsize)
		{
#ifdef EXT_DEP
		printf("\ncreate_ocp_qp_res: outside memory bounds!\n\n");
#endif
		exit(1);
		}
#endif


	return;

	}



hpipm_size_t OCP_QP_RES_WS_MEMSIZE(struct OCP_QP_DIM *dim)
	{

	// loop index
	int ii;

	// extract ocp qp size
	int N = dim->N;
	int *nx = dim->nx;
	int *nu = dim->nu;
	int *nb = dim->nb;
	int *ng = dim->ng;
	int *ns = dim->ns;

	// compute core qp size and max size
	int nuxM = 0;
	int nbM = 0;
	int ngM = 0;
	int nsM = 0;
	for(ii=0; ii<=N; ii++)
		{
		nbM = nb[ii]>nbM ? nb[ii] : nbM;
		ngM = ng[ii]>ngM ? ng[ii] : ngM;
		nsM = ns[ii]>nsM ? ns[ii] : nsM;
		}

	hpipm_size_t size = 0;

	size += 4*sizeof(struct STRVEC); // 2*tmp_nbgM tmp_nsM tmp_lam_mask

	size += 2*SIZE_STRVEC(nbM+ngM); // tmp_nbgM
	size += 1*SIZE_STRVEC(nsM); // tmp_nsM
	size += 1*SIZE_STRVEC(2*nbM+2*ngM+2*nsM); // tmp_lam_mask

	size = (size+63)/64*64; // make multiple of typical cache line size
	size += 1*64; // align once to typical cache line size

	return size;

	}



void OCP_QP_RES_WS_CREATE(struct OCP_QP_DIM *dim, struct OCP_QP_RES_WS *ws, void *mem)
	{

	// loop index
	int ii;

	// zero memory (to avoid corrupted memory like e.g. NaN)
	hpipm_size_t memsize = OCP_QP_RES_WS_MEMSIZE(dim);
	hpipm_zero_memset(memsize, mem);

	// extract ocp qp size
	int N = dim->N;
	int *nx = dim->nx;
	int *nu = dim->nu;
	int *nb = dim->nb;
	int *ng = dim->ng;
	int *ns = dim->ns;


	// compute core qp size and max size
	int nbM = 0;
	int ngM = 0;
	int nsM = 0;
	for(ii=0; ii<N; ii++)
		{
		nbM = nb[ii]>nbM ? nb[ii] : nbM;
		ngM = ng[ii]>ngM ? ng[ii] : ngM;
		nsM = ns[ii]>nsM ? ns[ii] : nsM;
		}
	nbM = nb[ii]>nbM ? nb[ii] : nbM;
	ngM = ng[ii]>ngM ? ng[ii] : ngM;
	nsM = ns[ii]>nsM ? ns[ii] : nsM;


	// vector struct
	struct STRVEC *sv_ptr = (struct STRVEC *) mem;

	ws->tmp_nbgM = sv_ptr;
	sv_ptr += 2;
	ws->tmp_nsM = sv_ptr;
	sv_ptr += 1;
	ws->tmp_lam_mask = sv_ptr;
	sv_ptr += 1;


	// align to typical cache line size
	hpipm_size_t s_ptr = (hpipm_size_t) sv_ptr;
	s_ptr = (s_ptr+63)/64*64;


	// void stuf
	char *c_ptr = (char *) s_ptr;

	CREATE_STRVEC(nbM+ngM, ws->tmp_nbgM+0, c_ptr);
	c_ptr += (ws->tmp_nbgM+0)->memsize;

	CREATE_STRVEC(nbM+ngM, ws->tmp_nbgM+1, c_ptr);
	c_ptr += (ws->tmp_nbgM+1)->memsize;

	CREATE_STRVEC(nsM, ws->tmp_nsM+0, c_ptr);
	c_ptr += (ws->tmp_nsM+0)->memsize;

	CREATE_STRVEC(2*nbM+2*ngM+2*nsM, ws->tmp_lam_mask, c_ptr);
	c_ptr += (ws->tmp_lam_mask)->memsize;

	ws->valid_nc_mask = 0;

	ws->memsize = memsize; //OCP_QP_RES_WS_MEMSIZE(dim);


#if defined(RUNTIME_CHECKS)
	if(c_ptr > ((char *) mem) + ws->memsize)
		{
#ifdef EXT_DEP
		printf("\ncreate_ocp_qp_res_workspace: outside memory bounds!\n\n");
#endif
		exit(1);
		}
#endif


	return;

	}



void OCP_QP_RES_COMPUTE(struct OCP_QP *qp, struct OCP_QP_SOL *qp_sol, struct OCP_QP_RES *res, struct OCP_QP_RES_WS *ws)
	{

	// loop index
	int ii, jj, idx;

	//
	int N = qp->dim->N;
	int *nx = qp->dim->nx;
	int *nu = qp->dim->nu;
	int *nb = qp->dim->nb;
	int *ng = qp->dim->ng;
	int *ns = qp->dim->ns;

	//int nct = 0;
	//for(ii=0; ii<=N; ii++)
	//	nct += 2*nb[ii]+2*ng[ii]+2*ns[ii];

	//REAL nct_inv = 1.0/nct;

	struct STRMAT *BAbt = qp->BAbt;
	struct STRMAT *RSQrq = qp->RSQrq;
	struct STRMAT *DCt = qp->DCt;
	struct STRVEC *b = qp->b;
	struct STRVEC *rqz = qp->rqz;
	struct STRVEC *d = qp->d;
	struct STRVEC *d_mask = qp->d_mask;
	struct STRVEC *m = qp->m;
	int **idxb = qp->idxb;
	struct STRVEC *Z = qp->Z;
	int **idxs_rev = qp->idxs_rev;

	struct STRVEC *ux = qp_sol->ux;
	struct STRVEC *pi = qp_sol->pi;
	struct STRVEC *lam = qp_sol->lam;
	struct STRVEC *t = qp_sol->t;

	struct STRVEC *res_g = res->res_g;
	struct STRVEC *res_b = res->res_b;
	struct STRVEC *res_d = res->res_d;
	struct STRVEC *res_m = res->res_m;

	struct STRVEC *tmp_nbgM = ws->tmp_nbgM;
	struct STRVEC *tmp_nsM = ws->tmp_nsM;
	struct STRVEC *tmp_lam_mask = ws->tmp_lam_mask;

	int nx0, nx1, nu0, nu1, nb0, ng0, ns0;

	REAL *obj = &res->obj;
	REAL *dual_gap = &res->dual_gap;

	int mask_constr = 0;
	REAL nc_mask_inv = 0.0;
	if(ws->valid_nc_mask==1)
		{
		mask_constr = ws->mask_constr;
		nc_mask_inv = ws->nc_mask_inv;
		}
	else
		{
		int nc_mask = 0;
		for(ii=0; ii<=N; ii++)
			for(jj=0; jj<2*nb[ii]+2*ng[ii]+2*ns[ii]; jj++)
				if((d_mask+ii)->pa[jj]==1.0)
					nc_mask++;
				else
					mask_constr = 1; // at least one masked constraint
		if(nc_mask>0)
			nc_mask_inv = 1.0/nc_mask;
		// do not store these in ws, to guard against changes in d_mask
		}

	//
	REAL mu = 0.0;
	*obj = 0.0;
	*dual_gap = 0.0;
	//REAL res_m_sum = 0.0; // ~ dual gap for primal-dual feasible points

	// loop over stages
	for(ii=0; ii<=N; ii++)
		{

		nx0 = nx[ii];
		nu0 = nu[ii];
		nb0 = nb[ii];
		ng0 = ng[ii];
		ns0 = ns[ii];

//		SYMV_L(nu0+nx0, 1.0, RSQrq+ii, 0, 0, ux+ii, 0, 1.0, rqz+ii, 0, res_g+ii, 0);
		SYMV_L(nu0+nx0, 1.0, RSQrq+ii, 0, 0, ux+ii, 0, 2.0, rqz+ii, 0, res_g+ii, 0);
		*obj += 0.5*DOT(nu0+nx0, res_g+ii, 0, ux+ii, 0);
		AXPY(nu0+nx0, -1.0, rqz+ii, 0, res_g+ii, 0, res_g+ii, 0);
		*dual_gap += DOT(nu0+nx0, res_g+ii, 0, ux+ii, 0);

		if(ii>0)
			AXPY(nx0, -1.0, pi+(ii-1), 0, res_g+ii, nu0, res_g+ii, nu0);

		if(mask_constr)
			VECMUL(2*nb0+2*ng0+2*ns0, lam+ii, 0, d_mask+ii, 0, tmp_lam_mask, 0);
		else
			VECCP(2*nb0+2*ng0+2*ns0, lam+ii, 0, tmp_lam_mask, 0);

		if(nb0+ng0>0)
			{
			AXPY(nb0+ng0, -1.0, tmp_lam_mask, 0, tmp_lam_mask, nb0+ng0, tmp_nbgM+0, 0);
//			AXPY(nb0+ng0,  1.0, d+ii, 0, t+ii, 0, res_d+ii, 0);
//			AXPY(nb0+ng0,  1.0, d+ii, nb0+ng0, t+ii, nb0+ng0, res_d+ii, nb0+ng0);
			AXPY(2*nb0+2*ng0,  1.0, d+ii, 0, t+ii, 0, res_d+ii, 0);
			// box
			if(nb0>0)
				{
				VECAD_SP(nb0, 1.0, tmp_nbgM+0, 0, idxb[ii], res_g+ii, 0);
				VECEX_SP(nb0, 1.0, idxb[ii], ux+ii, 0, tmp_nbgM+1, 0);
				}
			// general
			if(ng0>0)
				{
				GEMV_NT(nu0+nx0, ng0, 1.0, 1.0, DCt+ii, 0, 0, tmp_nbgM+0, nb[ii], ux+ii, 0, 1.0, 0.0, res_g+ii, 0, tmp_nbgM+1, nb0, res_g+ii, 0, tmp_nbgM+1, nb0);
				}

			AXPY(nb0+ng0, -1.0, tmp_nbgM+1, 0, res_d+ii, 0, res_d+ii, 0);
			AXPY(nb0+ng0,  1.0, tmp_nbgM+1, 0, res_d+ii, nb0+ng0, res_d+ii, nb0+ng0);
			}
		if(ns0>0)
			{
			// res_g
//			GEMV_DIAG(2*ns0, 1.0, Z+ii, 0, ux+ii, nu0+nx0, 1.0, rqz+ii, nu0+nx0, res_g+ii, nu0+nx0);
			GEMV_DIAG(2*ns0, 1.0, Z+ii, 0, ux+ii, nu0+nx0, 2.0, rqz+ii, nu0+nx0, res_g+ii, nu0+nx0);
			*obj += 0.5*DOT(2*ns0, res_g+ii, nu0+nx0, ux+ii, nu0+nx0);
			AXPY(2*ns0, -1.0, rqz+ii, nu0+nx0, res_g+ii, nu0+nx0, res_g+ii, nu0+nx0);
			*dual_gap += DOT(2*ns0, res_g+ii, nu0+nx0, ux+ii, nu0+nx0);

			AXPY(2*ns0, -1.0, tmp_lam_mask, 2*nb0+2*ng0, res_g+ii, nu0+nx0, res_g+ii, nu0+nx0);
			for(jj=0; jj<nb0+ng0; jj++)
				{
				idx = idxs_rev[ii][jj];
				if(idx!=-1)
					{
					BLASFEO_VECEL(res_g+ii, nu0+nx0+idx)     -= BLASFEO_VECEL(tmp_lam_mask, jj);
					BLASFEO_VECEL(res_g+ii, nu0+nx0+ns0+idx) -= BLASFEO_VECEL(tmp_lam_mask, nb0+ng0+jj);
					// res_d
					BLASFEO_VECEL(res_d+ii, jj)         -= BLASFEO_VECEL(ux+ii, nu0+nx0+idx);
					BLASFEO_VECEL(res_d+ii, nb0+ng0+jj) -= BLASFEO_VECEL(ux+ii, nu0+nx0+ns0+idx);
					}
				}
			// res_d
			AXPY(2*ns0, -1.0, ux+ii, nu0+nx0, t+ii, 2*nb0+2*ng0, res_d+ii, 2*nb0+2*ng0);
			AXPY(2*ns0, 1.0, d+ii, 2*nb0+2*ng0, res_d+ii, 2*nb0+2*ng0, res_d+ii, 2*nb0+2*ng0);
			}
		if(mask_constr)
			VECMUL(2*nb0+2*ng0+2*ns0, d_mask+ii, 0, res_d+ii, 0, res_d+ii, 0);

		*dual_gap -= DOT(2*nb0+2*ng0+2*ns0, d+ii, 0, tmp_lam_mask, 0);

		if(ii<N)
			{

			nu1 = nu[ii+1];
			nx1 = nx[ii+1];

			AXPY(nx1, -1.0, ux+(ii+1), nu1, b+ii, 0, res_b+ii, 0);

			GEMV_NT(nu0+nx0, nx1, 1.0, 1.0, BAbt+ii, 0, 0, pi+ii, 0, ux+ii, 0, 1.0, 1.0, res_g+ii, 0, res_b+ii, 0, res_g+ii, 0, res_b+ii, 0);
			*dual_gap -= DOT(nx1, b+ii, 0, pi+ii, 0);

			}

		mu += VECMULDOT(2*nb0+2*ng0+2*ns0, tmp_lam_mask, 0, t+ii, 0, res_m+ii, 0);
		AXPY(2*nb0+2*ng0+2*ns0, -1.0, m+ii, 0, res_m+ii, 0, res_m+ii, 0); // TODO not necessary if m is zero
		if(mask_constr)
			VECMUL(2*nb0+2*ng0+2*ns0, d_mask+ii, 0, res_m+ii, 0, res_m+ii, 0); // TODO not necessary if m is zero

		// TODO mask res_g for the slacks of the soft constraints ??? no !!!

		//for(jj=0; jj<2*nb0+2*ng0+2*ns0; jj++)
		//	{
		//	res_m_sum += BLASFEO_VECEL(res_m+ii, jj);
		//	}
		}

	//res->res_mu = mu*nct_inv;
	res->res_mu = mu*nc_mask_inv;
	res->res_mu_sum = mu;

	return;

	}



void OCP_QP_RES_COMPUTE_LIN (struct OCP_QP *qp, struct OCP_QP_SOL *qp_sol, struct OCP_QP_SOL *qp_step, struct OCP_QP_RES *res, struct OCP_QP_RES_WS *ws)
	{

	// loop index
	int ii, jj, idx;

	//
	int N = qp->dim->N;
	int *nx = qp->dim->nx;
	int *nu = qp->dim->nu;
	int *nb = qp->dim->nb;
	int *ng = qp->dim->ng;
	int *ns = qp->dim->ns;

	struct STRMAT *BAbt = qp->BAbt;
	struct STRMAT *RSQrq = qp->RSQrq;
	struct STRMAT *DCt = qp->DCt;
	struct STRVEC *b = qp->b;
	struct STRVEC *rqz = qp->rqz;
	struct STRVEC *d = qp->d;
	struct STRVEC *d_mask = qp->d_mask;
	struct STRVEC *m = qp->m;
	int **idxb = qp->idxb;
	struct STRVEC *Z = qp->Z;
	int **idxs_rev = qp->idxs_rev;

	struct STRVEC *ux = qp_step->ux;
	struct STRVEC *pi = qp_step->pi;
	struct STRVEC *lam = qp_step->lam;
	struct STRVEC *t = qp_step->t;

	struct STRVEC *Lam = qp_sol->lam;
	struct STRVEC *T = qp_sol->t;

	struct STRVEC *res_g = res->res_g;
	struct STRVEC *res_b = res->res_b;
	struct STRVEC *res_d = res->res_d;
	struct STRVEC *res_m = res->res_m;

	struct STRVEC *tmp_nbgM = ws->tmp_nbgM;
	struct STRVEC *tmp_nsM = ws->tmp_nsM;
	struct STRVEC *tmp_lam_mask = ws->tmp_lam_mask;

	int nx0, nx1, nu0, nu1, nb0, ng0, ns0;

	int mask_constr = 0;
	if(ws->valid_nc_mask==1)
		{
		mask_constr = ws->mask_constr;
		}
	else
		{
		for(ii=0; ii<=N; ii++)
			for(jj=0; jj<2*nb[ii]+2*ng[ii]+2*ns[ii]; jj++)
				if((d_mask+ii)->pa[jj]!=1.0)
					mask_constr = 1; // at least one masked constraint
		// do not store these in ws, to guard against changes in d_mask
		}

	//
	REAL mu = 0.0;

	// loop over stages
	for(ii=0; ii<=N; ii++)
		{

		nx0 = nx[ii];
		nu0 = nu[ii];
		nb0 = nb[ii];
		ng0 = ng[ii];
		ns0 = ns[ii];

		SYMV_L(nu0+nx0, 1.0, RSQrq+ii, 0, 0, ux+ii, 0, 1.0, rqz+ii, 0, res_g+ii, 0);

		if(ii>0)
			AXPY(nx0, -1.0, pi+(ii-1), 0, res_g+ii, nu0, res_g+ii, nu0);

		if(mask_constr)
			VECMUL(2*nb0+2*ng0+2*ns0, lam+ii, 0, d_mask+ii, 0, tmp_lam_mask, 0);
		else
			VECCP(2*nb0+2*ng0+2*ns0, lam+ii, 0, tmp_lam_mask, 0);

		if(nb0+ng0>0)
			{
			AXPY(nb0+ng0, -1.0, tmp_lam_mask, 0, tmp_lam_mask, nb[ii]+ng[ii], tmp_nbgM+0, 0);
//			AXPY(nb0+ng0,  1.0, d+ii, 0, t+ii, 0, res_d+ii, 0);
//			AXPY(nb0+ng0,  1.0, d+ii, nb0+ng0, t+ii, nb0+ng0, res_d+ii, nb0+ng0);
			AXPY(2*nb0+2*ng0,  1.0, d+ii, 0, t+ii, 0, res_d+ii, 0);
			// box
			if(nb0>0)
				{
				VECAD_SP(nb0, 1.0, tmp_nbgM+0, 0, idxb[ii], res_g+ii, 0);
				VECEX_SP(nb0, 1.0, idxb[ii], ux+ii, 0, tmp_nbgM+1, 0);
				}
			// general
			if(ng0>0)
				{
				GEMV_NT(nu0+nx0, ng0, 1.0, 1.0, DCt+ii, 0, 0, tmp_nbgM+0, nb[ii], ux+ii, 0, 1.0, 0.0, res_g+ii, 0, tmp_nbgM+1, nb0, res_g+ii, 0, tmp_nbgM+1, nb0);
				}

			AXPY(nb0+ng0, -1.0, tmp_nbgM+1, 0, res_d+ii, 0, res_d+ii, 0);
			AXPY(nb0+ng0,  1.0, tmp_nbgM+1, 0, res_d+ii, nb0+ng0, res_d+ii, nb0+ng0);
			}
		if(ns0>0)
			{
			// res_g
			GEMV_DIAG(2*ns0, 1.0, Z+ii, 0, ux+ii, nu0+nx0, 1.0, rqz+ii, nu0+nx0, res_g+ii, nu0+nx0);
			AXPY(2*ns0, -1.0, tmp_lam_mask, 2*nb0+2*ng0, res_g+ii, nu0+nx0, res_g+ii, nu0+nx0);
			for(jj=0; jj<nb0+ng0; jj++)
				{
				idx = idxs_rev[ii][jj];
				if(idx!=-1)
					{
					BLASFEO_VECEL(res_g+ii, nu0+nx0+idx) -= BLASFEO_VECEL(tmp_lam_mask, jj);
					BLASFEO_VECEL(res_g+ii, nu0+nx0+ns0+idx) -= BLASFEO_VECEL(tmp_lam_mask, nb0+ng0+jj);
					// res_d
					BLASFEO_VECEL(res_d+ii, jj) -= BLASFEO_VECEL(ux+ii, nu0+nx0+idx);
					BLASFEO_VECEL(res_d+ii, nb0+ng0+jj) -= BLASFEO_VECEL(ux+ii, nu0+nx0+ns0+idx);
					}
				}
			// res_d
			AXPY(2*ns0, -1.0, ux+ii, nu0+nx0, t+ii, 2*nb0+2*ng0, res_d+ii, 2*nb0+2*ng0);
			AXPY(2*ns0, 1.0, d+ii, 2*nb0+2*ng0, res_d+ii, 2*nb0+2*ng0, res_d+ii, 2*nb0+2*ng0);
			}

		if(mask_constr)
			VECMUL(2*nb0+2*ng0+2*ns0, d_mask+ii, 0, res_d+ii, 0, res_d+ii, 0);

		if(ii<N)
			{

			nu1 = nu[ii+1];
			nx1 = nx[ii+1];

			AXPY(nx1, -1.0, ux+(ii+1), nu1, b+ii, 0, res_b+ii, 0);

			GEMV_NT(nu0+nx0, nx1, 1.0, 1.0, BAbt+ii, 0, 0, pi+ii, 0, ux+ii, 0, 1.0, 1.0, res_g+ii, 0, res_b+ii, 0, res_g+ii, 0, res_b+ii, 0);

			}

		VECCP(2*nb0+2*ng0+2*ns0, m+ii, 0, res_m+ii, 0);
		VECMULACC(2*nb0+2*ng0+2*ns0, Lam+ii, 0, t+ii, 0, res_m+ii, 0);
		VECMULACC(2*nb0+2*ng0+2*ns0, lam+ii, 0, T+ii, 0, res_m+ii, 0);
		if(mask_constr)
			VECMUL(2*nb0+2*ng0+2*ns0, d_mask+ii, 0, res_m+ii, 0, res_m+ii, 0);

		}

	return;

	}



void OCP_QP_RES_COMPUTE_INF_NORM(struct OCP_QP_RES *res)
	{

	struct OCP_QP_DIM *dim = res->dim;
	int N = dim->N;
	int *nx = dim->nx;
	int *nu = dim->nu;
	int *nb = dim->nb;
	int *ng = dim->ng;
	int *ns = dim->ns;

	int ii;

	int nv = 0;
	int ne = 0;
	int nc = 0;

	for(ii=0; ii<N; ii++)
		{
		nv += nu[ii]+nx[ii]+2*ns[ii];
		ne += nx[ii+1];
		nc += 2*nb[ii]+2*ng[ii]+2*ns[ii];
		}
	ii = N;
	nv += nu[ii]+nx[ii]+2*ns[ii];
	nc += 2*nb[ii]+2*ng[ii]+2*ns[ii];

	// compute infinity norm
	VECNRM_INF(nv, res->res_g, 0, res->res_max+0);
	VECNRM_INF(ne, res->res_b, 0, res->res_max+1);
	VECNRM_INF(nc, res->res_d, 0, res->res_max+2);
	VECNRM_INF(nc, res->res_m, 0, res->res_max+3);

	return;

	}



void OCP_QP_RES_GET_ALL(struct OCP_QP_RES *res, REAL **res_r, REAL **res_q, REAL **res_ls, REAL **res_us, REAL **res_b, REAL **res_d_lb, REAL **res_d_ub, REAL **res_d_lg, REAL **res_d_ug, REAL **res_d_ls, REAL **res_d_us, REAL **res_m_lb, REAL **res_m_ub, REAL **res_m_lg, REAL **res_m_ug, REAL **res_m_ls, REAL **res_m_us)
	{

	int N = res->dim->N;
	int *nx = res->dim->nx;
	int *nu = res->dim->nu;
	int *nb = res->dim->nb;
	int *ng = res->dim->ng;
	int *ns = res->dim->ns;

	int ii;

	for(ii=0; ii<N; ii++)
		{
		// cost
		UNPACK_VEC(nu[ii], res->res_g+ii, 0, res_r[ii], 1);
		UNPACK_VEC(nx[ii], res->res_g+ii, nu[ii], res_q[ii], 1);

		// dynamics
		UNPACK_VEC(nx[ii+1], res->res_b+ii, 0, res_b[ii], 1);

		// box constraints
		if(nb[ii]>0)
			{
			UNPACK_VEC(nb[ii], res->res_d+ii, 0, res_d_lb[ii], 1);
			UNPACK_VEC(nb[ii], res->res_d+ii, nb[ii]+ng[ii], res_d_ub[ii], 1);
			UNPACK_VEC(nb[ii], res->res_m+ii, 0, res_m_lb[ii], 1);
			UNPACK_VEC(nb[ii], res->res_m+ii, nb[ii]+ng[ii], res_m_ub[ii], 1);
			}

		// general constraints
		if(ng[ii]>0)
			{
			UNPACK_VEC(ng[ii], res->res_d+ii, nb[ii], res_d_lg[ii], 1);
			UNPACK_VEC(ng[ii], res->res_d+ii, 2*nb[ii]+ng[ii], res_d_ug[ii], 1);
			UNPACK_VEC(ng[ii], res->res_m+ii, nb[ii], res_m_lg[ii], 1);
			UNPACK_VEC(ng[ii], res->res_m+ii, 2*nb[ii]+ng[ii], res_m_ug[ii], 1);
			}

		// soft constraints
		if(ns[ii]>0)
			{
			UNPACK_VEC(ns[ii], res->res_g+ii, nu[ii]+nx[ii], res_ls[ii], 1);
			UNPACK_VEC(ns[ii], res->res_g+ii, nu[ii]+nx[ii]+ns[ii], res_us[ii], 1);
			UNPACK_VEC(ns[ii], res->res_d+ii, 2*nb[ii]+2*ng[ii], res_d_ls[ii], 1);
			UNPACK_VEC(ns[ii], res->res_d+ii, 2*nb[ii]+2*ng[ii]+ns[ii], res_d_us[ii], 1);
			UNPACK_VEC(ns[ii], res->res_m+ii, 2*nb[ii]+2*ng[ii], res_m_ls[ii], 1);
			UNPACK_VEC(ns[ii], res->res_m+ii, 2*nb[ii]+2*ng[ii]+ns[ii], res_m_us[ii], 1);
			}
		}

	// cost
	UNPACK_VEC(nu[ii], res->res_g+ii, 0, res_r[ii], 1);
	UNPACK_VEC(nx[ii], res->res_g+ii, nu[ii], res_q[ii], 1);

	// box constraints
	if(nb[ii]>0)
		{
		UNPACK_VEC(nb[ii], res->res_d+ii, 0, res_d_lb[ii], 1);
		UNPACK_VEC(nb[ii], res->res_d+ii, nb[ii]+ng[ii], res_d_ub[ii], 1);
		UNPACK_VEC(nb[ii], res->res_m+ii, 0, res_m_lb[ii], 1);
		UNPACK_VEC(nb[ii], res->res_m+ii, nb[ii]+ng[ii], res_m_ub[ii], 1);
		}

	// general constraints
	if(ng[ii]>0)
		{
		UNPACK_VEC(ng[ii], res->res_d+ii, nb[ii], res_d_lg[ii], 1);
		UNPACK_VEC(ng[ii], res->res_d+ii, 2*nb[ii]+ng[ii], res_d_ug[ii], 1);
		UNPACK_VEC(ng[ii], res->res_m+ii, nb[ii], res_m_lg[ii], 1);
		UNPACK_VEC(ng[ii], res->res_m+ii, 2*nb[ii]+ng[ii], res_m_ug[ii], 1);
		}

	// soft constraints
	if(ns[ii]>0)
		{
		UNPACK_VEC(ns[ii], res->res_g+ii, nu[ii]+nx[ii], res_ls[ii], 1);
		UNPACK_VEC(ns[ii], res->res_g+ii, nu[ii]+nx[ii]+ns[ii], res_us[ii], 1);
		UNPACK_VEC(ns[ii], res->res_d+ii, 2*nb[ii]+2*ng[ii], res_d_ls[ii], 1);
		UNPACK_VEC(ns[ii], res->res_d+ii, 2*nb[ii]+2*ng[ii]+ns[ii], res_d_us[ii], 1);
		UNPACK_VEC(ns[ii], res->res_m+ii, 2*nb[ii]+2*ng[ii], res_m_ls[ii], 1);
		UNPACK_VEC(ns[ii], res->res_m+ii, 2*nb[ii]+2*ng[ii]+ns[ii], res_m_us[ii], 1);
		}

	return;

	}



void OCP_QP_RES_GET_MAX_RES_STAT(struct OCP_QP_RES *res, REAL *value)
	{

	*value = res->res_max[0];

	return;

	}



void OCP_QP_RES_GET_MAX_RES_EQ(struct OCP_QP_RES *res, REAL *value)
	{

	*value = res->res_max[1];

	return;

	}



void OCP_QP_RES_GET_MAX_RES_INEQ(struct OCP_QP_RES *res, REAL *value)
	{

	*value = res->res_max[2];

	return;

	}



void OCP_QP_RES_GET_MAX_RES_COMP(struct OCP_QP_RES *res, REAL *value)
	{

	*value = res->res_max[3];

	return;

	}



void OCP_QP_RES_SET_ZERO(struct OCP_QP_RES *res)
	{

	int N = res->dim->N;
	int *nx = res->dim->nx;
	int *nu = res->dim->nu;
	int *nb = res->dim->nb;
	int *ng = res->dim->ng;
	int *ns = res->dim->ns;

	int ii;

	// res_g
	for(ii=0; ii<=N; ii++)
		{
		VECSE(nu[ii]+nx[ii]+2*ns[ii], 0.0, res->res_g+ii, 0);
		}
	// res_b
	for(ii=0; ii<N; ii++)
		{
		VECSE(nx[ii+1], 0.0, res->res_b+ii, 0);
		}
	// res_d
	for(ii=0; ii<=N; ii++)
		{
		VECSE(2*nb[ii]+2*ng[ii]+2*ns[ii], 0.0, res->res_d+ii, 0);
		}
	// res_m
	for(ii=0; ii<=N; ii++)
		{
		VECSE(2*nb[ii]+2*ng[ii]+2*ns[ii], 0.0, res->res_m+ii, 0);
		}

	return;

	}



void OCP_QP_RES_SET(char *field, int stage, REAL *vec, struct OCP_QP_RES *res)
	{
	if(hpipm_strcmp(field, "res_r"))
		{
		OCP_QP_RES_SET_RES_R(stage, vec, res);
		}
	else if(hpipm_strcmp(field, "res_q"))
		{
		OCP_QP_RES_SET_RES_Q(stage, vec, res);
		}
	else if(hpipm_strcmp(field, "res_zl"))
		{
		OCP_QP_RES_SET_RES_ZL(stage, vec, res);
		}
	else if(hpipm_strcmp(field, "res_zu"))
		{
		OCP_QP_RES_SET_RES_ZU(stage, vec, res);
		}
	else if(hpipm_strcmp(field, "res_b"))
		{
		OCP_QP_RES_SET_RES_B(stage, vec, res);
		}
	else if(hpipm_strcmp(field, "res_lb"))
		{
		OCP_QP_RES_SET_RES_LB(stage, vec, res);
		}
	else if(hpipm_strcmp(field, "res_lbu"))
		{
		OCP_QP_RES_SET_RES_LBU(stage, vec, res);
		}
	else if(hpipm_strcmp(field, "res_lbx"))
		{
		OCP_QP_RES_SET_RES_LBX(stage, vec, res);
		}
	else if(hpipm_strcmp(field, "res_ub"))
		{
		OCP_QP_RES_SET_RES_UB(stage, vec, res);
		}
	else if(hpipm_strcmp(field, "res_ubu"))
		{
		OCP_QP_RES_SET_RES_UBU(stage, vec, res);
		}
	else if(hpipm_strcmp(field, "res_ubx"))
		{
		OCP_QP_RES_SET_RES_UBX(stage, vec, res);
		}
	else if(hpipm_strcmp(field, "res_lg"))
		{
		OCP_QP_RES_SET_RES_LG(stage, vec, res);
		}
	else if(hpipm_strcmp(field, "res_ug"))
		{
		OCP_QP_RES_SET_RES_UG(stage, vec, res);
		}
	else if(hpipm_strcmp(field, "res_ls"))
		{
		OCP_QP_RES_SET_RES_LS(stage, vec, res);
		}
	else if(hpipm_strcmp(field, "res_us"))
		{
		OCP_QP_RES_SET_RES_US(stage, vec, res);
		}
	else
		{
#ifdef EXT_DEP
		printf("error [OCP_QP_RES_SET]: unknown field name '%s'. Exiting.\n", field);
#endif
		exit(1);
		}
	return;
	}



void OCP_QP_RES_SET_RES_R(int stage, REAL *vec, struct OCP_QP_RES *res)
	{
	int *nu = res->dim->nu;
	PACK_VEC(nu[stage], vec, 1, res->res_g+stage, 0);
	return;
	}



void OCP_QP_RES_SET_RES_Q(int stage, REAL *vec, struct OCP_QP_RES *res)
	{
	int *nu = res->dim->nu;
	int *nx = res->dim->nx;
	PACK_VEC(nx[stage], vec, 1, res->res_g+stage, nu[stage]);
	return;
	}



void OCP_QP_RES_SET_RES_ZL(int stage, REAL *vec, struct OCP_QP_RES *res)
	{
	int *nu = res->dim->nu;
	int *nx = res->dim->nx;
	int *ns = res->dim->ns;
	PACK_VEC(ns[stage], vec, 1, res->res_g+stage, nu[stage]+nx[stage]);
	return;
	}



void OCP_QP_RES_SET_RES_ZU(int stage, REAL *vec, struct OCP_QP_RES *res)
	{
	int *nu = res->dim->nu;
	int *nx = res->dim->nx;
	int *ns = res->dim->ns;
	PACK_VEC(ns[stage], vec, 1, res->res_g+stage, nu[stage]+nx[stage]+ns[stage]);
	return;
	}



void OCP_QP_RES_SET_RES_B(int stage, REAL *vec, struct OCP_QP_RES *res)
	{
	int *nx = res->dim->nx;
	PACK_VEC(nx[stage+1], vec, 1, res->res_b+stage, 0);
	}



void OCP_QP_RES_SET_RES_LB(int stage, REAL *vec, struct OCP_QP_RES *res)
	{
	int *nb = res->dim->nb;
	PACK_VEC(nb[stage], vec, 1, res->res_d+stage, 0);
	}



void OCP_QP_RES_SET_RES_LBU(int stage, REAL *vec, struct OCP_QP_RES *res)
	{
	int *nbu = res->dim->nbu;
	PACK_VEC(nbu[stage], vec, 1, res->res_d+stage, 0);
	}



void OCP_QP_RES_SET_RES_LBX(int stage, REAL *vec, struct OCP_QP_RES *res)
	{
	int *nbu = res->dim->nbu;
	int *nbx = res->dim->nbx;
	PACK_VEC(nbx[stage], vec, 1, res->res_d+stage, nbu[stage]);
	}



void OCP_QP_RES_SET_RES_UB(int stage, REAL *vec, struct OCP_QP_RES *res)
	{
	int *nb = res->dim->nb;
	int *ng = res->dim->ng;
	PACK_VEC(nb[stage], vec, 1, res->res_d+stage, nb[stage]+ng[stage]);
	}



void OCP_QP_RES_SET_RES_UBU(int stage, REAL *vec, struct OCP_QP_RES *res)
	{
	int *nb = res->dim->nb;
	int *nbu = res->dim->nbu;
	int *ng = res->dim->ng;
	PACK_VEC(nbu[stage], vec, 1, res->res_d+stage, nb[stage]+ng[stage]);
	}



void OCP_QP_RES_SET_RES_UBX(int stage, REAL *vec, struct OCP_QP_RES *res)
	{
	int *nb = res->dim->nb;
	int *nbu = res->dim->nbu;
	int *nbx = res->dim->nbx;
	int *ng = res->dim->ng;
	PACK_VEC(nbx[stage], vec, 1, res->res_d+stage, nb[stage]+ng[stage]+nbu[stage]);
	}



void OCP_QP_RES_SET_RES_LG(int stage, REAL *vec, struct OCP_QP_RES *res)
	{
	int *nb = res->dim->nb;
	int *ng = res->dim->ng;
	PACK_VEC(ng[stage], vec, 1, res->res_d+stage, nb[stage]);
	}



void OCP_QP_RES_SET_RES_UG(int stage, REAL *vec, struct OCP_QP_RES *res)
	{
	int *nb = res->dim->nb;
	int *ng = res->dim->ng;
	PACK_VEC(ng[stage], vec, 1, res->res_d+stage, 2*nb[stage]+ng[stage]);
	}



void OCP_QP_RES_SET_RES_LS(int stage, REAL *vec, struct OCP_QP_RES *res)
	{
	int *nb = res->dim->nb;
	int *ng = res->dim->ng;
	int *ns = res->dim->ns;
	PACK_VEC(ns[stage], vec, 1, res->res_d+stage, 2*nb[stage]+2*ng[stage]);
	}



void OCP_QP_RES_SET_RES_US(int stage, REAL *vec, struct OCP_QP_RES *res)
	{
	int *nb = res->dim->nb;
	int *ng = res->dim->ng;
	int *ns = res->dim->ns;
	PACK_VEC(ns[stage], vec, 1, res->res_d+stage, 2*nb[stage]+2*ng[stage]+ns[stage]);
	}



