// FloatCCNode.cpp
// Build with Maya C++ API (e.g., MSVC/Clang + Maya devkit).
#include <maya/MPxNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnPlugin.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MGlobal.h>

class FloatCCNode : public MPxNode {
public:
    FloatCCNode() = default;
    ~FloatCCNode() override = default;

    static void* creator() { return new FloatCCNode(); }
    static MStatus initialize();
    MStatus compute(const MPlug& plug, MDataBlock& data) override;

    static MTypeId id;               // 替换为你项目的唯一ID
    static MObject inValue;          // 输入
    static MObject outValue;         // 输出
};

MTypeId FloatCCNode::id(0x001226D1);   // TODO: 请换成你申请/分配的唯一ID
MObject  FloatCCNode::inValue;
MObject  FloatCCNode::outValue;

MStatus FloatCCNode::initialize() {
    MStatus stat;
    MFnNumericAttribute nAttr;

    inValue = nAttr.create("inValue", "in", MFnNumericData::kFloat, 0.0f, &stat);
    CHECK_MSTATUS_AND_RETURN_IT(stat);
    nAttr.setStorable(true);
    nAttr.setKeyable(true);
    nAttr.setReadable(true);
    nAttr.setWritable(true);

    outValue = nAttr.create("outValue", "out", MFnNumericData::kFloat, 0.0f, &stat);
    CHECK_MSTATUS_AND_RETURN_IT(stat);
    nAttr.setStorable(false);
    nAttr.setKeyable(false);
    nAttr.setReadable(true);
    nAttr.setWritable(false);

    stat = addAttribute(inValue);    CHECK_MSTATUS_AND_RETURN_IT(stat);
    stat = addAttribute(outValue);   CHECK_MSTATUS_AND_RETURN_IT(stat);

    // 建立依赖关系：inValue 变化驱动 outValue
    stat = attributeAffects(inValue, outValue); CHECK_MSTATUS_AND_RETURN_IT(stat);

    return MS::kSuccess;
}

MStatus FloatCCNode::compute(const MPlug& plug, MDataBlock& data) {
    if (plug != outValue) return MS::kUnknownParameter;

    float inV = data.inputValue(inValue).asFloat();
    MDataHandle outH = data.outputValue(outValue);
    outH.setFloat(inV);        // 恒等传递，也可改为其他函数关系
    outH.setClean();

    return MS::kSuccess;
}

// 插件入口
MStatus initializePlugin(MObject obj) {
    MStatus stat;
    MFnPlugin plugin(obj, "YourNameOrStudio", "1.0.0", "Any");

    stat = plugin.registerNode(
        "floatcc",                // 节点名称（createNode 使用的名字）
        FloatCCNode::id,          // 唯一类型ID（务必替换）
        FloatCCNode::creator,
        FloatCCNode::initialize,
        MPxNode::kDependNode
    );
    if (!stat) stat.perror("registerNode floatcc");
    return stat;
}

// 插件卸载
MStatus uninitializePlugin(MObject obj) {
    MStatus stat;
    MFnPlugin plugin(obj);
    stat = plugin.deregisterNode(FloatCCNode::id);
    if (!stat) stat.perror("deregisterNode floatcc");
    return stat;
}
