classdef MomentumFreeFloatingJacobian < iDynTree.MatrixDynSize
  methods
    function self = MomentumFreeFloatingJacobian(varargin)
      self@iDynTree.MatrixDynSize(iDynTreeSwigRef.Null);
      if nargin==1 && strcmp(class(varargin{1}),'iDynTreeSwigRef')
        if ~isnull(varargin{1})
          self.swigPtr = varargin{1}.swigPtr;
        end
      else
        tmp = iDynTreeMEX(1241, varargin{:});
        self.swigPtr = tmp.swigPtr;
        tmp.SwigClear();
      end
    end
    function varargout = resize(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(1242, self, varargin{:});
    end
    function varargout = isConsistent(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(1243, self, varargin{:});
    end
    function delete(self)
      if self.swigPtr
        iDynTreeMEX(1244, self);
        self.SwigClear();
      end
    end
  end
  methods(Static)
  end
end
