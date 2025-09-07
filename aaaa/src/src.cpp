#include <maya/MFnPlugin.h>
#include <maya/MPxNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNurbsCurveData.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MFnDagNode.h>
#include <maya/MVector.h>
#include <maya/MPoint.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MFnData.h>
#include <maya/MGlobal.h>
#include <maya/MTypeId.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MVectorArray.h>

class GetControlPointWorldTranslateNode : public MPxNode
{
public:
    GetControlPointWorldTranslateNode() {}
    virtual ~GetControlPointWorldTranslateNode() {}
    virtual MStatus compute(const MPlug& plug, MDataBlock& data) override;

    static void* creator() { return new GetControlPointWorldTranslateNode(); }
    static MStatus initialize();

public:
    static MTypeId id;
    static MObject inputCurve;  // 输入曲线
    static MObject outputCurve; // 输出曲线
    static MObject outputCPs;   // 输出control points数组
};

MTypeId GetControlPointWorldTranslateNode::id(0x001226C0);

// Attributes
MObject GetControlPointWorldTranslateNode::inputCurve;
MObject GetControlPointWorldTranslateNode::outputCurve;
MObject GetControlPointWorldTranslateNode::outputCPs;

MStatus GetControlPointWorldTranslateNode::initialize()
{
    MStatus stat;

    // 输入曲线
    MFnTypedAttribute tAttr;
    inputCurve = tAttr.create("inputCurve", "inCrv", MFnData::kNurbsCurve, MObject::kNullObj, &stat);
    tAttr.setStorable(true);
    addAttribute(inputCurve);

    // 输出曲线
    outputCurve = tAttr.create("outputCurve", "outCrv", MFnData::kNurbsCurve, MObject::kNullObj, &stat);
    tAttr.setWritable(false);
    tAttr.setStorable(false);
    addAttribute(outputCurve);

    // 输出 control points
    MFnTypedAttribute cpAttr;
    outputCPs = cpAttr.create("outputCPs", "outCPs", MFnData::kVectorArray, MObject::kNullObj, &stat);
    cpAttr.setArray(true);      // 可扩展数组
    cpAttr.setUsesArrayDataBuilder(true);
    cpAttr.setWritable(false);
    cpAttr.setStorable(false);
    addAttribute(outputCPs);

    // 依赖关系
    attributeAffects(inputCurve, outputCurve);
    attributeAffects(inputCurve, outputCPs);

    return MS::kSuccess;
}

MStatus GetControlPointWorldTranslateNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;

    if (plug != outputCurve && plug.parent() != outputCPs)
        return MS::kUnknownParameter;

    // 获取输入曲线
    MObject inCurveObj = data.inputValue(inputCurve, &status).asNurbsCurve();
    if (status != MStatus::kSuccess || inCurveObj.isNull())
        return MStatus::kFailure;

    MFnNurbsCurve fnCurve(inCurveObj);

    // 生成输出曲线
    MFnNurbsCurveData curveDataFn;
    MObject outCurveData = curveDataFn.create();
    MObject newCurve = fnCurve.copy(outCurveData);

    MDataHandle outCurveHandle = data.outputValue(outputCurve);
    outCurveHandle.set(newCurve);

    // 输出control points世界坐标
    MVectorArray worldCPs;
    unsigned int numCVs = fnCurve.numCVs();
    for (unsigned int i = 0; i < numCVs; i++)
    {
        MPoint pt;
        fnCurve.getCV(i, pt, MSpace::kWorld);
        worldCPs.append(MVector(pt.x, pt.y, pt.z));
    }

    MFnVectorArrayData vecData;
    MObject outVecData = vecData.create(worldCPs);
    MDataHandle outCPHandle = data.outputValue(outputCPs);
    outCPHandle.set(outVecData);

    data.setClean(plug);
    return MStatus::kSuccess;
}

// 插件注册
MStatus initializePlugin(MObject obj)
{
    MFnPlugin plugin(obj, "Your Name", "1.0", "Any");

    // 注册节点
    plugin.registerNode(
        "getControlPointWorldTranslate",
        GetControlPointWorldTranslateNode::id,
        GetControlPointWorldTranslateNode::creator,
        GetControlPointWorldTranslateNode::initialize,
        MPxNode::kDependNode
    );

    return MS::kSuccess;
}

MStatus uninitializePlugin(MObject obj)
{
    MFnPlugin plugin(obj);

    plugin.deregisterCommand("helloWorld");
    plugin.deregisterNode(GetControlPointWorldTranslateNode::id);

    return MS::kSuccess;
}
