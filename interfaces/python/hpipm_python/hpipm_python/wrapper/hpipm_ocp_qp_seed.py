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

from ctypes import *
import numpy as np



class hpipm_ocp_qp_seed:


	def __init__(self, dim):

		# save dim internally
		self.dim = dim

		# load hpipm shared library
		__hpipm   = CDLL('libhpipm.so')
		self.__hpipm = __hpipm

		# C qp struct
		qp_seed_struct_size = __hpipm.d_ocp_qp_seed_strsize()
		qp_seed_struct = cast(create_string_buffer(qp_seed_struct_size), c_void_p)
		self.qp_seed_struct = qp_seed_struct

		# C qp internal memory
		qp_seed_mem_size = __hpipm.d_ocp_qp_seed_memsize(dim.dim_struct)
		qp_seed_mem = cast(create_string_buffer(qp_seed_mem_size), c_void_p)
		self.qp_seed_mem = qp_seed_mem

		# create C qp
		__hpipm.d_ocp_qp_seed_create(dim.dim_struct, qp_seed_struct, qp_seed_mem)


	def set(self, field, value, idx_start, idx_end=None):
		# checks on index
		NN = np.zeros((1,1), dtype=int)
		tmp_ptr = cast(NN.ctypes.data, POINTER(c_int))
		self.__hpipm.d_ocp_qp_dim_get_N(self.dim.dim_struct, tmp_ptr)
		N = NN[0,0]
		if idx_start<0:
			raise IndexError('hpipm_ocp_qp_seed.set: wrong idx_start')
		if field=='pi':
			if idx_start>N-1:
				raise IndexError('hpipm_ocp_qp_seed.set: wrong idx_start')
		else:
			if idx_start>N:
				raise IndexError('hpipm_ocp_qp_seed.set: wrong idx_start')
		if idx_end is None or idx_end<idx_start:
			idx_end_ = idx_start
		else:
			if field=='pi':
				if idx_end>N-1:
					raise IndexError('hpipm_ocp_qp_seed.set: wrong idx_end')
			else:
				if idx_end>N:
					raise IndexError('hpipm_ocp_qp_seed.set: wrong idx_end')
			idx_end_ = idx_end
		# value
		value = np.ascontiguousarray(value, dtype=np.float64)
		tmp = cast(value.ctypes.data, POINTER(c_double))
		field_enc = field.encode('utf-8')
		if idx_end is None:
			idx_end = idx_start
		for i in range(idx_start, idx_end+1):
			self.__hpipm.d_ocp_qp_seed_set(c_char_p(field_enc), i, tmp, self.qp_seed_struct)
		return


	def print_C_struct(self):
		self.__hpipm.d_ocp_qp_seed_print(self.dim.dim_struct, self.qp_seed_struct)
		return
