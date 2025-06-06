###################################################################################################
#                                                                                                 #
# This file is part of HPIPM.                                                                     #
#                                                                                                 #
# HPIPM -- High-Performance Interior Point Method.                                                #
# Copyright (C) 2019 by Gianluca Frison.                                                          #
# Developed at IMTEK (University of Freiburg) under the supervision of Moritz Diehl.              #
# All rights reserved.                                                                            #
#                                                                                                 #
# The 2-Clause BSD License                                                                        #
#                                                                                                 #
# Redistribution and use in source and binary forms, with or without                              #
# modification, are permitted provided that the following conditions are met:                     #
#                                                                                                 #
# 1. Redistributions of source code must retain the above copyright notice, this                  #
#    list of conditions and the following disclaimer.                                             #
# 2. Redistributions in binary form must reproduce the above copyright notice,                    #
#    this list of conditions and the following disclaimer in the documentation                    #
#    and/or other materials provided with the distribution.                                       #
#                                                                                                 #
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND                 #
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED                   #
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE                          #
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR                 #
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES                  #
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;                    #
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND                     #
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT                      #
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS                   #
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                                    #
#                                                                                                 #
# Author: Gianluca Frison, gianluca.frison (at) imtek.uni-freiburg.de                             #
#                                                                                                 #
###################################################################################################


include ../Makefile.rule

OBJS =

ifeq ($(TARGET), GENERIC)
OBJS +=
endif

# double
OBJS += d_dense_qp_dim.o 
OBJS += d_dense_qp.o 
OBJS += d_dense_qp_sol.o 
OBJS += d_dense_qp_res.o 
OBJS += d_dense_qp_kkt.o 
OBJS += d_dense_qp_ipm.o
OBJS += d_dense_qp_utils.o
OBJS += d_dense_qp_seed.o
OBJS += d_dense_qcqp_dim.o 
OBJS += d_dense_qcqp.o 
OBJS += d_dense_qcqp_sol.o 
OBJS += d_dense_qcqp_res.o 
OBJS += d_dense_qcqp_ipm.o 
OBJS += d_dense_qcqp_utils.o 
# single
OBJS += s_dense_qp_dim.o 
OBJS += s_dense_qp.o 
OBJS += s_dense_qp_sol.o 
OBJS += s_dense_qp_res.o 
OBJS += s_dense_qp_kkt.o 
OBJS += s_dense_qp_ipm.o
OBJS += s_dense_qp_utils.o
OBJS += s_dense_qp_seed.o
OBJS += s_dense_qcqp_dim.o 
OBJS += s_dense_qcqp.o 
OBJS += s_dense_qcqp_sol.o 
OBJS += s_dense_qcqp_res.o 
OBJS += s_dense_qcqp_ipm.o 
OBJS += s_dense_qcqp_utils.o 
# mixed
OBJS += m_dense_qp_dim.o 
OBJS += m_dense_qp.o 

obj: $(OBJS)

clean:
	rm -f *.o
	rm -f *.s

d_dense_qp_dim.o: d_dense_qp_dim.c x_dense_qp_dim.c
s_dense_qp_dim.o: s_dense_qp_dim.c x_dense_qp_dim.c
d_dense_qp.o: d_dense_qp.c x_dense_qp.c
s_dense_qp.o: s_dense_qp.c x_dense_qp.c
d_dense_qp_sol.o: d_dense_qp_sol.c x_dense_qp_sol.c
s_dense_qp_sol.o: s_dense_qp_sol.c x_dense_qp_sol.c
d_dense_qp_res.o: d_dense_qp_res.c x_dense_qp_res.c
s_dense_qp_res.o: s_dense_qp_res.c x_dense_qp_res.c
d_dense_qp_kkt.o: d_dense_qp_kkt.c x_dense_qp_kkt.c
s_dense_qp_kkt.o: s_dense_qp_kkt.c x_dense_qp_kkt.c
d_dense_qp_ipm.o: d_dense_qp_ipm.c x_dense_qp_ipm.c
s_dense_qp_ipm.o: s_dense_qp_ipm.c x_dense_qp_ipm.c
d_dense_qp_utils.o: d_dense_qp_utils.c x_dense_qp_utils.c
s_dense_qp_utils.o: s_dense_qp_utils.c x_dense_qp_utils.c
d_dense_qp_seed.o: d_dense_qp_seed.c x_dense_qp_seed.c
s_dense_qp_seed.o: s_dense_qp_seed.c x_dense_qp_seed.c
d_dense_qcqp_dim.o: d_dense_qcqp_dim.c x_dense_qcqp_dim.c
s_dense_qcqp_dim.o: s_dense_qcqp_dim.c x_dense_qcqp_dim.c
d_dense_qcqp.o: d_dense_qcqp.c x_dense_qcqp.c
s_dense_qcqp.o: s_dense_qcqp.c x_dense_qcqp.c
d_dense_qcqp_sol.o: d_dense_qcqp_sol.c x_dense_qcqp_sol.c
s_dense_qcqp_sol.o: s_dense_qcqp_sol.c x_dense_qcqp_sol.c
d_dense_qcqp_res.o: d_dense_qcqp_res.c x_dense_qcqp_res.c
s_dense_qcqp_res.o: s_dense_qcqp_res.c x_dense_qcqp_res.c
d_dense_qcqp_ipm.o: d_dense_qcqp_ipm.c x_dense_qcqp_ipm.c
s_dense_qcqp_ipm.o: s_dense_qcqp_ipm.c x_dense_qcqp_ipm.c
d_dense_qcqp_utils.o: d_dense_qcqp_utils.c x_dense_qcqp_utils.c
s_dense_qcqp_utils.o: s_dense_qcqp_utils.c x_dense_qcqp_utils.c
