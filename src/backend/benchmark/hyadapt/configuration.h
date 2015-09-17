//===----------------------------------------------------------------------===//
//
//                         PelotonDB
//
// configuration.h
//
// Identification: benchmark/hyadapt/configuration.h
//
// Copyright (c) 2015, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <string>
#include <getopt.h>
#include <vector>
#include <sys/time.h>
#include <iostream>

#include "backend/storage/data_table.h"

namespace peloton {
namespace benchmark {
namespace hyadapt{

enum OperatorType{
  OPERATOR_TYPE_INVALID = 0,         /* invalid */

  OPERATOR_TYPE_DIRECT = 1,          /* direct */
  OPERATOR_TYPE_AGGREGATE = 2,       /* aggregate */
  OPERATOR_TYPE_ARITHMETIC = 3       /* arithmetic */
};

class configuration {
 public:

  OperatorType operator_type;

  // size of the table
  int scale_factor;

  // tile group layout
  LayoutType layout;

  double selectivity;

  double projectivity;

  // # of times to run operator
  int transactions;

};

void usage_exit(FILE *out);

void parse_arguments(int argc, char* argv[], configuration& state);

}  // namespace hyadapt
}  // namespace benchmark
}  // namespace peloton
