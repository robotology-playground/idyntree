classdef GeomVector3Semantics__LinearMotionVector3Semantics < SwigRef
  methods
    function this = swig_this(self)
      this = iDynTreeMEX(3, self);
    end
    function self = GeomVector3Semantics__LinearMotionVector3Semantics(varargin)
      if nargin==1 && strcmp(class(varargin{1}),'SwigRef')
        if ~isnull(varargin{1})
          self.swigPtr = varargin{1}.swigPtr;
        end
      else
        tmp = iDynTreeMEX(286, varargin{:});
        self.swigPtr = tmp.swigPtr;
        tmp.swigPtr = [];
      end
    end
    function varargout = setToUnknown(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(287, self, varargin{:});
    end
    function varargout = getBody(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(288, self, varargin{:});
    end
    function varargout = getRefBody(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(289, self, varargin{:});
    end
    function varargout = getCoordinateFrame(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(290, self, varargin{:});
    end
    function varargout = isUnknown(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(291, self, varargin{:});
    end
    function varargout = changeCoordFrame(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(292, self, varargin{:});
    end
    function varargout = dot(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(295, self, varargin{:});
    end
    function delete(self)
      if self.swigPtr
        iDynTreeMEX(296, self);
        self.swigPtr=[];
      end
    end
  end
  methods(Static)
    function varargout = compose(varargin)
     [varargout{1:nargout}] = iDynTreeMEX(293, varargin{:});
    end
    function varargout = inverse(varargin)
     [varargout{1:nargout}] = iDynTreeMEX(294, varargin{:});
    end
  end
end
