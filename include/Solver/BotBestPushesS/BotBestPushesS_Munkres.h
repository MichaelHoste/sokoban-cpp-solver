/*
 *   Copyright (c) 2007 John Weaver
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

#if !defined(_BOTBESTPUSHESS_MUNKRES_H_)
#define _BOTBESTPUSHESS_MUNKRES_H_

#include "BotBestPushesS_Matrix.h"

#include <list>
#include <utility>

class BotBestPushesS_Munkres {
public:
	int* solve(BotBestPushesS_Matrix<double> &m);
private:
	inline bool find_uncovered_in_matrix(double,int&,int&);
	inline bool pair_in_list(const std::pair<int,int> &, const std::list<std::pair<int,int> > &);
	int step1(void);
	int step2(void);
	int step3(void);
	int step4(void);
	int step5(void);
	int step6(void);
	BotBestPushesS_Matrix<int> mask_matrix;
	BotBestPushesS_Matrix<double> matrix;
	bool *row_mask;
	bool *col_mask;
	int saverow, savecol;
};

#endif /* !defined(_BOTBESTPUSHESS_MUNKRES_H_) */
