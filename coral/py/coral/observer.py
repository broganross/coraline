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

import utils

class Observer(object):
    def __init__(self):
        self._notificationCallback = utils.weakRef(None)
        self._collector = utils.weakRef(None)
        self._data = {}
        self._id = id(self)
        self._subject = None
        
    def setData(self, key, value):
        self._data[key] = value
    
    def data(self, key):
        return self._data[key]
    
    def setNotificationCallback(self, callback):
        self._notificationCallback = utils.weakRef(callback)
    
    def notify(self):
        if self._notificationCallback():
            self._notificationCallback()()
    
    def __del__(self):
        collector = self._collector()
        if collector:
            collector.removeDeadObserver(self._id, self._subject)

class ObserverCollector(object):
    def __init__(self):
        self._list = {}
        self._dict = {}
    
    def add(self, observer, subject = None):
        observerRef = utils.weakRef(observer)
        if observerRef not in self._list.values():
            observer._collector = utils.weakRef(self)
            self._list[observer._id] = observerRef
            
        if subject:
            dictList = {}
            observer._subject = subject
            if self._dict.has_key(subject):
                dictList = self._dict[subject]
            else:
                self._dict[subject] = dictList
            
            dictList[observer._id] = observerRef
    
    def removeDeadObserver(self, observerId, subject = None):
        del self._list[observerId]
        
        if subject:
            del self._dict[subject][observerId]
        
    def observers(self, subject = None):
        list = self._list
        
        if subject:
            if self._dict.has_key(subject):
                list = self._dict[subject]
            else:
                list = {}
        
        observersCopy = []
        for observerRef in list.values():
            observersCopy.append(observerRef())
            
        return observersCopy
    
    
    