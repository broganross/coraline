
from maya import cmds, OpenMaya, OpenMayaAnim

def exportSkeleton(topNode, filename):
    fileContent = "coralIO:1.0\n"
    fileContent += "type:transform\n"
    
    joints = cmds.listRelatives(topNode, allDescendents = True, type = "joint")
    
    fileContent += "transforms:" + str(len(joints)) + "\n"
    
    start = cmds.playbackOptions(query = True, animationStartTime = True)
    end = cmds.playbackOptions(query = True, animationEndTime = True)
    
    fileContent += "frames:" + str(int(end - start)) + "\n"
    
    for frame in range(start, end):
        cmds.currentTime(frame)
        for joint in joints:
            xform = cmds.xform(joint, query = True, worldSpace = True, matrix = True)
            fileContent += str(xform).strip("[]").replace(", ", ",") + "\n"
    
    file = open(filename, "w")
    file.write(fileContent)
    file.close()
    
    print "coralIO: saved file " + filename

def exportSkinWeights(topNode, skinClusterNode, filename):
    fileContent = "coralIO:1.0\n"
    fileContent += "type:skinWeight\n"

    sel = OpenMaya.MSelectionList()
    sel.add(skinClusterNode)
    skinClusterObj = OpenMaya.MObject()
    sel.getDependNode(0, skinClusterObj)

    skinClusterFn = OpenMayaAnim.MFnSkinCluster(skinClusterObj)
    skinPath = OpenMaya.MDagPath()
    skinClusterFn.getPathAtIndex(0, skinPath)

    influences = OpenMaya.MDagPathArray()
    skinClusterFn.influenceObjects(influences)

    joints = cmds.listRelatives(topNode, allDescendents = True, type = "joint", fullPath = True)

    jointsMap = {}
    for i in range(len(joints)):
        jointsMap[joints[i]] = i

    geoIt = OpenMaya.MItGeometry(skinPath)

    fileContent += "vertices:" + str(geoIt.count()) + "\n"
    fileContent += "deformers:" + str(influences.length()) + "\n"

    for i in range(geoIt.count()):
        comp = geoIt.component()

        weights = OpenMaya.MFloatArray()
        influenceIds = OpenMaya.MIntArray()

        infCount = OpenMaya.MScriptUtil()
        ccc = infCount.asUintPtr()

        skinClusterFn.getWeights(skinPath, comp, weights, ccc)
        for j in range(weights.length()):
            weight = weights[j]
            if weight > 0.0:
                influenceName = influences[j].fullPathName()
                jointId = jointsMap[influenceName]
                fileContent += "vertex:" + str(i) + ",deformer:" + str(jointId) + ",weight:" + str(weights[j]) + "\n"

        geoIt.next()
    
    file = open(filename, "w")
    file.write(fileContent)
    file.close()
    
    print "coralIO: saved file " + filename

