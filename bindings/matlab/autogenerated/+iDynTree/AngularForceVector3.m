classdef AngularForceVector3 < iDynTree.ForceVector3__AngularForceVector3
  methods
    function self = AngularForceVector3(varargin)
      self@iDynTree.ForceVector3__AngularForceVector3(SwigRef.Null);
      if nargin==1 && strcmp(class(varargin{1}),'SwigRef')
        if ~isnull(varargin{1})
          self.swigPtr = varargin{1}.swigPtr;
        end
      else
        tmp = iDynTreeMEX(502, varargin{:});
        self.swigPtr = tmp.swigPtr;
        tmp.swigPtr = [];
      end
    end
    function varargout = changePoint(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(503, self, varargin{:});
    end
    function delete(self)
      if self.swigPtr
        iDynTreeMEX(504, self);
        self.swigPtr=[];
      end
    end
  end
  methods(Static)
  end
end
