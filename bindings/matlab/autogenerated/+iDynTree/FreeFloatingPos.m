classdef FreeFloatingPos < SwigRef
  methods
    function this = swig_this(self)
      this = iDynTreeMEX(3, self);
    end
    function self = FreeFloatingPos(varargin)
      if nargin==1 && strcmp(class(varargin{1}),'SwigRef')
        if ~isnull(varargin{1})
          self.swigPtr = varargin{1}.swigPtr;
        end
      else
        tmp = iDynTreeMEX(1134, varargin{:});
        self.swigPtr = tmp.swigPtr;
        tmp.swigPtr = [];
      end
    end
    function varargout = resize(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(1135, self, varargin{:});
    end
    function varargout = worldBasePos(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(1136, self, varargin{:});
    end
    function varargout = jointPos(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(1137, self, varargin{:});
    end
    function varargout = getNrOfPosCoords(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(1138, self, varargin{:});
    end
    function delete(self)
      if self.swigPtr
        iDynTreeMEX(1139, self);
        self.swigPtr=[];
      end
    end
  end
  methods(Static)
  end
end
