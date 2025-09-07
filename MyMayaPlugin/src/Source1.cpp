#include <maya/MFnPlugin.h>
#include <maya/MPxCommand.h>
#include <maya/MGlobal.h>

// 命令类
class HelloWorldCmd : public MPxCommand
{
public:
    HelloWorldCmd() {}
    virtual MStatus doIt(const MArgList&) override {
        MGlobal::displayInfo("Hello World from my Maya plugin!");
        return MS::kSuccess;
    }

    static void* creator() {
        return new HelloWorldCmd();
    }
};

// 插件入口
MStatus initializePlugin(MObject obj)
{
    MFnPlugin plugin(obj, "Your Name", "1.0", "Any");
    // 注册命令
    plugin.registerCommand("helloWorld", HelloWorldCmd::creator);
    return MS::kSuccess;
}

// 插件卸载
MStatus uninitializePlugin(MObject obj)
{
    MFnPlugin plugin(obj);
    // 注销命令
    plugin.deregisterCommand("helloWorld");
    return MS::kSuccess;
}
