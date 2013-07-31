# <license>
# Copyright (C) 2011 Andrea Interguglielmi, All rights reserved.
# This file is part of the coral repository downloaded from http://code.google.com/p/coral-repo.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
# 
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
# 
#    * Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
# IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# </license>

from plugin import Plugin
import _coral
import timeNode


def loadPlugin():
    plugin = Plugin("builtinNodes")
    
    plugin.registerAttribute("NumericAttribute", _coral.NumericAttribute)
    
    plugin.registerNode("Int", _coral.IntNode, tags = ["numeric"], description = "Generate an int.")
    plugin.registerNode("Float", _coral.FloatNode, tags = ["numeric"], description = "Generate a float.")
    plugin.registerNode("Vec3", _coral.Vec3Node, tags = ["numeric"], description = "Generate a vec3.\nWorks with single or array values.")
    plugin.registerNode("Vec3ToFloats", _coral.Vec3ToFloats, tags = ["numeric"], description = "Get x, y, z values from a vec3.\nWorks with single or array values.")
    plugin.registerNode("Col4", _coral.Col4Node, tags = ["numeric"], description = "Generate a col4.\nWorks with single or array values.")
    plugin.registerNode("Col4ToFloats", _coral.Col4ToFloats, tags = ["numeric"], description = "Get r, g, b, a values from a col4.\nWorks with single or array values.")
    plugin.registerNode("Col4Reverse", _coral.Col4Reverse, tags = ["numeric"], description = "Get 1-rgb. Alpha doesn't change.\nWorks with single or array values.")
    plugin.registerNode("Quat", _coral.QuatNode, tags = ["numeric"], description = "Generate a quaternion.\nWorks with single or array values.")
    plugin.registerNode("QuatToFloats", _coral.QuatToFloats, tags = ["numeric"], description = "Get r, x, y, z values from a quaternion.\nWorks with single or array values.")
    plugin.registerNode("QuatToAxisAngle", _coral.QuatToAxisAngle, tags = ["numeric"], description = "Convert a quaternion to axis/angle.")
    plugin.registerNode("QuatToEulerRotation", _coral.QuatToEulerRotation, tags = ["numeric"], description = "Convert a quaternion to euler rotation.")
    plugin.registerNode("QuatToMatrix44", _coral.QuatToMatrix44, tags = ["numeric"], description = "Convert a quaternion to a rotation matrix.")
    plugin.registerNode("Matrix44", _coral.Matrix44Node, tags = ["numeric"], description = "Generate a Matrix44.")
    plugin.registerNode("ConstantArray", _coral.ConstantArray, tags = ["numeric"], description = "Generate an array from a constant value.")
    plugin.registerNode("ArraySize", _coral.ArraySize, tags = ["numeric"], description = "Get the size of a numeric array")
    plugin.registerNode("BuildArray", _coral.BuildArray, tags = ["numeric"], description = "Build a numeric array by adding each individual element.")
    plugin.registerNode("RangeArray", _coral.RangeArray, tags = ["numeric"], description = "Generate an array from the given range, each element of the array is a progressive value of the range.")
    plugin.registerNode("Matrix44Translation", _coral.Matrix44Translation, tags = ["numeric"], description = "Get the translation values of a matrix44.\nWorks with single or array values.")
    plugin.registerNode("Matrix44RotationAxis", _coral.Matrix44RotationAxis, tags = ["numeric"], description = "Get each rotation axis of a matrix44 as three individual vectors.\nWorks with single or array values.")
    plugin.registerNode("Matrix44FromVectors", _coral.Matrix44FromVectors, tags = ["numeric"], description = "Build a matrix44 from vec3 values.\nWorks with single or array values.")
    plugin.registerNode("Matrix44EulerRotation", _coral.Matrix44EulerRotation, tags = ["numeric"], description = "Get the euler angles from a matrix44.\nWorks with single or array values.")
    plugin.registerNode("Matrix44ToQuat", _coral.Matrix44ToQuat, tags = ["numeric"], description = "Get the quaternion from a matrix44.\nWorks with single or array values.")
    plugin.registerNode("RangeLoop", _coral.RangeLoop, tags = ["numeric"], description = "Generate a value that will loop in a given range of values.\nWorks with single or array values.")
    plugin.registerNode("RandomNumber", _coral.RandomNumber, tags = ["numeric"], description = "Generate a random number.\nWorks with single or array values.")
    plugin.registerNode("ArrayIndices", _coral.ArrayIndices, tags = ["numeric", "loop"], description = "Build an array composed by all the indexes extracted from the given input array.")
    plugin.registerNode("GetArrayElement", _coral.GetArrayElement, tags = ["numeric"], description = "Get a single element of an array.")
    plugin.registerNode("SetArrayElement", _coral.SetArrayElement, tags = ["numeric"], description = "Set a single element of an array.")
    plugin.registerNode("GetSimulationStep", _coral.GetSimulationStep, tags = ["numeric", "simulation"], description = "Get the values stored by SetSimulationStep and reuse them in the simulation step.\nWhen the step attribute is set to 0 the simulation is reset and the data is taken from the source.")
    plugin.registerNode("SetSimulationStep", _coral.SetSimulationStep, tags = ["numeric", "simulation"], description = "Set some numeric values and make them available to a GetSimulationStep node connected to the same source.\n")
    plugin.registerNode("FindPointsInRange", _coral.FindPointsInRange, tags = ["numeric"])

    plugin.registerNode("Add", _coral.AddNode, tags = ["math"])
    plugin.registerNode("Sub", _coral.SubNode, tags = ["math"])
    plugin.registerNode("Mul", _coral.MulNode, tags = ["math"])
    plugin.registerNode("Div", _coral.DivNode, tags = ["math"])
    plugin.registerNode("Abs", _coral.Abs, tags = ["math"])
    plugin.registerNode("Atan2", _coral.Atan2, tags = ["math"])
    plugin.registerNode("Sqrt", _coral.Sqrt, tags = ["math"])
    plugin.registerNode("Pow", _coral.Pow, tags = ["math"])
    plugin.registerNode("Exp", _coral.Exp, tags = ["math"])
    plugin.registerNode("Log", _coral.Log, tags = ["math"])
    plugin.registerNode("Ceil", _coral.Ceil, tags = ["math"])
    plugin.registerNode("Floor", _coral.Floor, tags = ["math"])
    plugin.registerNode("Round", _coral.Round, tags = ["math"])
    plugin.registerNode("Length", _coral.Length, tags = ["math"])
    plugin.registerNode("Inverse", _coral.Inverse, tags = ["math"])
    plugin.registerNode("CrossProduct", _coral.CrossProduct, tags = ["math"])
    plugin.registerNode("DotProduct", _coral.DotProduct, tags = ["math"])
    plugin.registerNode("Normalize", _coral.Normalize, tags = ["math"])
    plugin.registerNode("TrigonometricFunc", _coral.TrigonometricFunc, tags = ["math"])
    plugin.registerNode("Radians", _coral.Radians, tags = ["math"])
    plugin.registerNode("Degrees", _coral.Degrees, tags = ["math"])
    plugin.registerNode("Min", _coral.Min, tags = ["math"])
    plugin.registerNode("Max", _coral.Max, tags = ["math"])
    plugin.registerNode("Average", _coral.Average, tags = ["math"])
    plugin.registerNode("Slerp", _coral.Slerp, tags = ["math"])
    plugin.registerNode("QuatMultiply", _coral.QuatMultiply, tags = ["math"])
    plugin.registerNode("Negate", _coral.Negate, tags = ["math"])
    
    plugin.registerAttribute("ImageAttribute", _coral.ImageAttribute)
    plugin.registerNode("Image", _coral.ImageNode, tags = ["image"])
    
    plugin.registerAttribute("GeoAttribute", _coral.GeoAttribute)
    plugin.registerAttribute("GeoInstanceArrayAttribute", _coral.GeoInstanceArrayAttribute)
    plugin.registerNode("SetGeoPoints", _coral.SetGeoPoints, tags = ["geometry"])
    plugin.registerNode("GetGeoPoints", _coral.GetGeoPoints, tags = ["geometry"])
    plugin.registerNode("GetGeoNormals", _coral.GetGeoNormals, tags = ["geometry"], description = "Get one normal vector per vertex.")
    plugin.registerNode("ObjImporter", _coral.ObjImporter, tags = ["geometry"])
    plugin.registerNode("GeoGrid", _coral.GeoGrid, tags = ["geometry"])
    plugin.registerNode("GeoSphere", _coral.GeoSphere, tags = ["geometry"])
    plugin.registerNode("GeoCube", _coral.GeoCube, tags = ["geometry"])
    plugin.registerNode("GeoNeighbourPoints", _coral.GeoNeighbourPoints, tags = ["geometry"])
    plugin.registerNode("GetGeoElements", _coral.GetGeoElements, tags = ["geometry"])
    plugin.registerNode("GetGeoSubElements", _coral.GetGeoSubElements, tags = ["geometry"])
    plugin.registerNode("GeoInstanceGenerator", _coral.GeoInstanceGenerator, tags = ["geometry"])
    
    plugin.registerAttribute("StringAttribute", _coral.StringAttribute)
    plugin.registerNode("String", _coral.StringNode, tags = ["generic"])
    plugin.registerNode("Time", timeNode.TimeNode, tags = ["generic"])
    plugin.registerNode("ImportCIOTransforms", _coral.ImportCIOTransforms, tags = ["generic"])
    plugin.registerNode("ImportCIOSkinWeights", _coral.ImportCIOSkinWeights, tags = ["generic"])
    
    plugin.registerNode("LoopInput", _coral.LoopInputNode, tags = ["loop"])
    plugin.registerNode("LoopOutput", _coral.LoopOutputNode, tags = ["loop"])
    plugin.registerNode("ForLoop", _coral.ForLoopNode, tags = ["loop"])
    
    plugin.registerNode("ProcessSimulation", _coral.ProcessSimulationNode, tags = ["generic", "simulation"])
    
    plugin.registerAttribute("BoolAttribute", _coral.BoolAttribute)
    plugin.registerNode("Bool", _coral.BoolNode, tags = ["conditional"])
    plugin.registerNode("IfGreaterThan", _coral.IfGreaterThan, tags = ["conditional"])
    plugin.registerNode("IfLessThan", _coral.IfLessThan, tags = ["conditional"])
    plugin.registerNode("ConditionalValue", _coral.ConditionalValue, tags = ["conditional"])
    
    plugin.registerNode("SplinePoint", _coral.SplinePoint, tags = ["curve"])
    
    plugin.registerNode("SkinWeightDeformer", _coral.SkinWeightDeformer, tags = ["deformers"])
    
    return plugin
