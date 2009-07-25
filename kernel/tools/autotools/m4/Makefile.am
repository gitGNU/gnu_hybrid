##
## Input file for automake
##
## Copyright (C) 2007, 2008 Francesco Salvestrini
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License along
## with this program; if not, write to the Free Software Foundation, Inc.,
## 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
##

include $(top_srcdir)/Makefile.sub

##
## Maintainer related targets
##
HOST = "http://git.savannah.gnu.org/cgit/autoconf-archive.git/plain"

DOWNLOAD =						\
	ax_split_version.m4				\
	ax_with_prog.m4					\
	ax_prog_bison.m4				\
	ax_prog_flex.m4					\
	ax_compare_version.m4				\
	ax_cflags_gcc_option.m4				\
	ax_missing_prog.m4				\
	ax_path_missing.m4				\
	ax_check_awk__v.m4				\
	ax_check_awk__x_escapes.m4			\
	ax_check_awk_and.m4				\
	ax_check_awk_argind.m4				\
	ax_check_awk_array_delete.m4			\
	ax_check_awk_array_delete_elem.m4		\
	ax_check_awk_array_in.m4			\
	ax_check_awk_asort.m4				\
	ax_check_awk_asorti.m4				\
	ax_check_awk_associative_array.m4		\
	ax_check_awk_atan2.m4				\
	ax_check_awk_compl.m4				\
	ax_check_awk_conditional_expression.m4		\
	ax_check_awk_cos.m4				\
	ax_check_awk_environ.m4				\
	ax_check_awk_errno.m4				\
	ax_check_awk_exit.m4				\
	ax_check_awk_exp.m4				\
	ax_check_awk_gensub.m4				\
	ax_check_awk_getline.m4				\
	ax_check_awk_gsub.m4				\
	ax_check_awk_ignorecase.m4			\
	ax_check_awk_index.m4				\
	ax_check_awk_int.m4				\
	ax_check_awk_length.m4				\
	ax_check_awk_log.m4				\
	ax_check_awk_lshift.m4				\
	ax_check_awk_match_2parms.m4			\
	ax_check_awk_match_3parms.m4			\
	ax_check_awk_operator_multiply_multiply.m4	\
	ax_check_awk_operator_square.m4			\
	ax_check_awk_or.m4				\
	ax_check_awk_printf.m4				\
	ax_check_awk_rand.m4				\
	ax_check_awk_rshift.m4				\
	ax_check_awk_sin.m4				\
	ax_check_awk_split.m4				\
	ax_check_awk_sprintf.m4				\
	ax_check_awk_sqrt.m4				\
	ax_check_awk_srand.m4				\
	ax_check_awk_strftime.m4			\
	ax_check_awk_strtonum.m4			\
	ax_check_awk_sub.m4				\
	ax_check_awk_substr.m4				\
	ax_check_awk_system.m4				\
	ax_check_awk_systime.m4				\
	ax_check_awk_tolower.m4				\
	ax_check_awk_toupper.m4				\
	ax_check_awk_user_defined_functions.m4		\
	ax_check_awk_var_regexp.m4			\
	ax_check_awk_variable_value_pairs.m4		\
	ax_check_awk_xor.m4				\
	ax_try_awk_anyout.m4				\
	ax_try_awk_expout.m4				\
	ax_gcc_version.m4				\
	ax_gxx_version.m4				\
	ax_gcc_option.m4

MACROS =					\
	$(DOWNLOAD)				\
	ax_awk_functionality.m4			\
	ax_arg_with.m4				\
	ax_asm_uscore.m4			\
	ax_blacklist_version.m4			\
	ax_c___asm__.m4				\
	ax_c___attribute__.m4			\
	ax_c___attribute___deprecated.m4	\
	ax_c___attribute___noreturn.m4		\
	ax_c___attribute___section.m4		\
	ax_c___attribute___unused.m4		\
	ax_c___builtin_expect.m4		\
	ax_c_check_flag.m4			\
	ax_c_concatenation.m4			\
	ax_c_empty_structs.m4			\
	ax_c_var___date__.m4			\
	ax_c_var___file__.m4			\
	ax_c_var___function__.m4		\
	ax_c_var___line__.m4			\
	ax_c_var___pretty_func__.m4		\
	ax_c_var___time__.m4			\
	ax_check_symbolic_links.m4		\
	ax_check_vpath_build.m4			\
	ax_cpp_check_flag.m4			\
	ax_cxx_bool.m4				\
	ax_cxx_check_flag.m4			\
	ax_cxx_dtor_after_atexit.m4		\
	ax_cxx_dynamic_cast.m4			\
	ax_cxx_exceptions.m4			\
	ax_cxx_namespaces.m4			\
	ax_cxx_rtti.m4				\
	ax_cxx_static_cast.m4			\
	ax_cxx_templates.m4			\
	ax_cxx_typename.m4			\
	ax_cxxcpp_check_flag.m4			\
	ax_getopt_long.m4			\
	ax_ld_check_flag.m4			\
	ax_remove_forbidden.m4			\
	ax_string_clean.m4			\
	ax_string_intersect.m4			\
	ax_test.m4				\
	ax_prog_grub1.m4			\
	ax_prog_grub2.m4

update-local: Makefile
	rm -r -f fetch-dir && \
	mkdir fetch-dir && \
	for i in $(DOWNLOAD) ; do \
		$(FETCH) $(HOST)/$$i fetch-dir/$$i || { \
			rm -f fetch-dir/$$i ; \
			exit 1 ; \
		} \
	done && \
	for i in $(DOWNLOAD) ; do \
		mv fetch-dir/$$i $$i || { \
			exit 1 ; \
		} \
	done && \
	rm -r -f fetch-dir

maintainer-check-local: Makefile
