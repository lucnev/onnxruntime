// Copyright (C) 2019-2022 Intel Corporation
// Licensed under the MIT License

#pragma once
#include <unordered_set>
#include <utility>
#include <map>
#include <set>
#include <vector>
#include <string>
#include "interface/graph/graph.h"

namespace onnxruntime {
namespace openvino_ep {

using VarianceFunc = std::function<bool(const onnxruntime::interface::NodeViewRef*)>;

enum versionNum {
  V_2020_4,
  V_2021_1,
  V_2021_2,
  V_2021_3,
  V_2021_4,
  V_2022_1,
  V_2022_2,
  V_2022_3,
  V_2023_0,
  V_2023_1,
};

using VersionNum = enum versionNum;

struct supportedOp {
  std::string optype;
  VersionNum version;
  std::vector<std::string> device_type;
};

struct unsupportedOpMode {
  std::vector<VersionNum> ver;
  VarianceFunc func;
};

using SupportedOp = struct supportedOp;
using UnsupportedOpMode = struct unsupportedOpMode;
using Pairs = std::pair<VersionNum, int>;

class DataOps {
 private:
  const onnxruntime::interface::GraphViewRef& graph_viewer_;
  VersionNum version_id_;
  std::string device_id_;
  std::multimap<std::string, UnsupportedOpMode> op_list_;
  std::vector<SupportedOp> subgraph_supported_;
  std::vector<SupportedOp> no_dimension_supported_;
  std::set<Pairs> supported_types_vpu_;
  std::set<Pairs> supported_types_cpu_;
  std::set<Pairs> supported_types_gpu_;
  std::set<Pairs> supported_types_initializer_;

 protected:
  virtual void populate_op_mode_supported();
  virtual void populate_types_supported();
  bool op_is_supported(std::string name, std::vector<SupportedOp>& list);
  bool dimension_unsupported(const onnxruntime::interface::NodeViewRef* node);
  bool unsupported_op_mode(const onnxruntime::interface::NodeViewRef* node);
  bool type_is_supported(const onnxruntime::interface::ValueInfoViewRef* node_arg, bool is_initializer);
  bool node_is_supported(const std::map<std::string,
                                        std::set<std::string>>& op_map,
                         const interface::NodeViewRef* node);

 public:
  DataOps(const onnxruntime::interface::GraphViewRef& graph_viewer_param, VersionNum ver, std::string dev_id) : graph_viewer_(graph_viewer_param), version_id_(ver), device_id_(dev_id) {
    populate_op_mode_supported();
    populate_types_supported();
  }

  virtual std::vector<interface::NodeViewRef*> GetUnsupportedNodeIndices(std::unordered_set<std::string>& ng_required_initializers);
  virtual bool IsOpSupportedOnlyInModel(std::string name);
  virtual bool SpecialConditionForClusterSizeOne(std::unordered_set<std::string>& ng_required_initializers, const onnxruntime::interface::NodeViewRef* node);
  virtual bool DoNotOmitSubGraph(const std::string& name);
  virtual bool InsertNode(const std::string& name);
  VersionNum GetVersion() const { return version_id_; }
};

}  // namespace openvino_ep
}  // namespace onnxruntime
