//===----------------------------------------------------------------------===//
//
//                         PelotonDB
//
// mapper.cpp
//
// Identification: src/backend/bridge/dml/mapper/mapper.cpp
//
// Copyright (c) 2015, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <cstring>
#include <cassert>

#include "backend/bridge/dml/mapper/mapper.h"
#include "backend/bridge/dml/executor/plan_executor.h"
#include "backend/planner/insert_plan.h"

#include "nodes/print.h"
#include "nodes/pprint.h"
#include "utils/lsyscache.h"
#include "parser/parsetree.h"

namespace peloton {
namespace bridge {

const PlanTransformer::TransformOptions PlanTransformer::DefaultOptions;

/**
 * @brief Convert Postgres Plan (tree) into AbstractPlan (tree).
 * @return Pointer to the constructed AbstractPlan Node.
 */
planner::AbstractPlan *PlanTransformer::TransformPlan(
    AbstractPlanState *planstate,
    const TransformOptions options) {

  assert(planstate);

  // Ignore empty plans
  if (planstate == nullptr) return nullptr;

  planner::AbstractPlan *peloton_plan = nullptr;

  switch (nodeTag(planstate)) {
    case T_ModifyTableState:
      peloton_plan = PlanTransformer::TransformModifyTable(
          reinterpret_cast<const ModifyTablePlanState *>(planstate),
          options);
      break;
    case T_SeqScanState:
      peloton_plan = PlanTransformer::TransformSeqScan(
          reinterpret_cast<const SeqScanPlanState *>(planstate),
          options);
      break;
    case T_IndexScanState:
      peloton_plan = PlanTransformer::TransformIndexScan(
          reinterpret_cast<const IndexScanPlanState *>(planstate),
          options);
      break;
    case T_IndexOnlyScanState:
      peloton_plan = PlanTransformer::TransformIndexOnlyScan(
          reinterpret_cast<const IndexOnlyScanPlanState *>(planstate),
          options);
      break;
    case T_BitmapHeapScanState:
      peloton_plan = PlanTransformer::TransformBitmapHeapScan(
          reinterpret_cast<const BitmapHeapScanPlanState *>(planstate),
          options);
      break;
    case T_LockRowsState:
      peloton_plan = PlanTransformer::TransformLockRows(
          reinterpret_cast<const LockRowsPlanState *>(planstate));
      break;
    case T_LimitState:
      peloton_plan = PlanTransformer::TransformLimit(
          reinterpret_cast<const LimitPlanState *>(planstate));
      break;
    case T_MergeJoinState:
      peloton_plan = PlanTransformer::TransformMergeJoin(
          reinterpret_cast<const MergeJoinPlanState *>(planstate));
      break;
    case T_HashJoinState:
    case T_NestLoopState:
      peloton_plan = PlanTransformer::TransformNestLoop(
          reinterpret_cast<const NestLoopPlanState *>(planstate));
      break;
    case T_MaterialState:
      peloton_plan = PlanTransformer::TransformMaterialization(
          reinterpret_cast<const MaterialPlanState *>(planstate));
      break;
    case T_AggState:
      peloton_plan = PlanTransformer::TransformAgg(
          reinterpret_cast<const AggPlanState*>(planstate));
      break;


    case T_SortState:
      peloton_plan = PlanTransformer::TransformSort(
          reinterpret_cast<const SortPlanState*>(planstate));
      /* no break */
    default: {
      LOG_ERROR("PlanTransformer :: Unsupported Postgres Plan Tag: %u ",
                nodeTag(planstate));
      elog(INFO, "Query: ");
      break;
    }
  }

  return peloton_plan;
}

/**
 * @brief Recursively destroy the nodes in a plan tree.
 */
bool PlanTransformer::CleanPlan(planner::AbstractPlan *root) {
  if (!root)
    return false;

  // Clean all children subtrees
  auto children = root->GetChildren();
  for (auto child : children) {
    auto rv = CleanPlan(child);
    assert(rv);
  }

  // Clean the root
  delete root;
  return true;
}

}  // namespace bridge
}  // namespace peloton
