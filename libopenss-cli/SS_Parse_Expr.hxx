/*******************************************************************************
 ** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
 ** Copyright (c) 2006-2010 Krell Institute  All Rights Reserved.
 **
 ** This library is free software; you can redistribute it and/or modify it under
 ** the terms of the GNU Lesser General Public License as published by the Free
 ** Software Foundation; either version 2.1 of the License, or (at your option)
 ** any later version.
 **
 ** This library is distributed in the hope that it will be useful, but WITHOUT
 ** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 ** FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 ** details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with this library; if not, write to the Free Software Foundation, Inc.,
 ** 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *******************************************************************************/

/** @file
*
 * Define the expression operations that can be specfied by user commands
 * and are used in the generation of views.
 *
 */

typedef enum {
    EXPRESSION_OP_ERROR,
    EXPRESSION_OP_CONST,
    EXPRESSION_OP_HEADER,
    EXPRESSION_OP_UMINUS,
    EXPRESSION_OP_ABS,
    EXPRESSION_OP_ADD,
    EXPRESSION_OP_SUB,
    EXPRESSION_OP_MULT,
    EXPRESSION_OP_DIV,
    EXPRESSION_OP_MOD,
    EXPRESSION_OP_MIN,
    EXPRESSION_OP_MAX,
    EXPRESSION_OP_SQRT,
    EXPRESSION_OP_STDEV,
    EXPRESSION_OP_PERCENT,
    EXPRESSION_OP_CONDEXP,
    EXPRESSION_OP_A_ADD,
    EXPRESSION_OP_A_MULT,
    EXPRESSION_OP_A_MIN,
    EXPRESSION_OP_A_MAX,
    EXPRESSION_OP_UINT,
    EXPRESSION_OP_INT,
    EXPRESSION_OP_FLOAT,
    EXPRESSION_OP_RATIO
} expression_operation_t;

inline bool Can_Accumulate (expression_operation_t op) {
  switch (op) {
    case EXPRESSION_OP_A_ADD:
    case EXPRESSION_OP_A_MULT:
    case EXPRESSION_OP_A_MIN:
    case EXPRESSION_OP_A_MAX:
      return true;
  }
  return false;
}

inline std::string ExprOperator (expression_operation_t op) {
  std::string S = std::string("unknown");
  switch (op) {
    case EXPRESSION_OP_ERROR: S = std::string("error"); break;
    case EXPRESSION_OP_CONST: S = std::string("const"); break;
    case EXPRESSION_OP_HEADER: S = std::string("header"); break;
    case EXPRESSION_OP_UMINUS: S = std::string("uminus"); break;
    case EXPRESSION_OP_ABS: S = std::string("abs"); break;
    case EXPRESSION_OP_ADD: S = std::string("add"); break;
    case EXPRESSION_OP_SUB: S = std::string("sub"); break;
    case EXPRESSION_OP_MULT: S = std::string("mult"); break;
    case EXPRESSION_OP_DIV: S = std::string("div"); break;
    case EXPRESSION_OP_MOD: S = std::string("mod"); break;
    case EXPRESSION_OP_MIN: S = std::string("min"); break;
    case EXPRESSION_OP_MAX: S = std::string("max"); break;
    case EXPRESSION_OP_SQRT: S = std::string("sqrt"); break;
    case EXPRESSION_OP_STDEV: S = std::string("stdev"); break;
    case EXPRESSION_OP_PERCENT: S = std::string("percent"); break;
    case EXPRESSION_OP_CONDEXP: S = std::string("condexp"); break;
    case EXPRESSION_OP_A_ADD: S = std::string("a_add"); break;
    case EXPRESSION_OP_A_MULT: S = std::string("a_mult"); break;
    case EXPRESSION_OP_A_MIN: S = std::string("a_min"); break;
    case EXPRESSION_OP_A_MAX: S = std::string("a_max"); break;
    case EXPRESSION_OP_UINT: S = std::string("uint"); break;
    case EXPRESSION_OP_INT: S = std::string("int"); break;
    case EXPRESSION_OP_FLOAT: S = std::string("float"); break;
    case EXPRESSION_OP_RATIO: S = std::string("ratio"); break;
  }
  return S;
}
