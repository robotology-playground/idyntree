classdef SpatialMotionVectorSemanticsBase < SwigRef
  methods
    function this = swig_this(self)
      this = iDynTreeMEX(3, self);
    end
    function self = SpatialMotionVectorSemanticsBase(varargin)
      if nargin==1 && strcmp(class(varargin{1}),'SwigRef')
        if ~isnull(varargin{1})
          self.swigPtr = varargin{1}.swigPtr;
        end
      else
        tmp = iDynTreeMEX(505, varargin{:});
        self.swigPtr = tmp.swigPtr;
        tmp.swigPtr = [];
      end
    end
    function varargout = check_linear2angularConsistency(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(506, self, varargin{:});
    end
    function varargout = toString(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(507, self, varargin{:});
    end
    function varargout = display(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(508, self, varargin{:});
    end
    function delete(self)
      if self.swigPtr
        iDynTreeMEX(509, self);
        self.swigPtr=[];
      end
    end
  end
  methods(Static)
  end
end
