classdef LinearMotionVector3 < iDynTree.MotionVector3__LinearMotionVector3
  methods
    function self = LinearMotionVector3(varargin)
      self@iDynTree.MotionVector3__LinearMotionVector3(SwigRef.Null);
      if nargin==1 && strcmp(class(varargin{1}),'SwigRef')
        if ~isnull(varargin{1})
          self.swigPtr = varargin{1}.swigPtr;
        end
      else
        tmp = iDynTreeMEX(486, varargin{:});
        self.swigPtr = tmp.swigPtr;
        tmp.swigPtr = [];
      end
    end
    function varargout = changePoint(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(487, self, varargin{:});
    end
    function delete(self)
      if self.swigPtr
        iDynTreeMEX(488, self);
        self.swigPtr=[];
      end
    end
  end
  methods(Static)
  end
end
