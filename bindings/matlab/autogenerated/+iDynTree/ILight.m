classdef ILight < SwigRef
  methods
    function this = swig_this(self)
      this = iDynTreeMEX(3, self);
    end
    function delete(self)
      if self.swigPtr
        iDynTreeMEX(1644, self);
        self.swigPtr=[];
      end
    end
    function varargout = getName(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(1645, self, varargin{:});
    end
    function varargout = setType(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(1646, self, varargin{:});
    end
    function varargout = getType(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(1647, self, varargin{:});
    end
    function varargout = setPosition(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(1648, self, varargin{:});
    end
    function varargout = getPosition(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(1649, self, varargin{:});
    end
    function varargout = setDirection(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(1650, self, varargin{:});
    end
    function varargout = getDirection(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(1651, self, varargin{:});
    end
    function varargout = setAmbientColor(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(1652, self, varargin{:});
    end
    function varargout = getAmbientColor(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(1653, self, varargin{:});
    end
    function varargout = setSpecularColor(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(1654, self, varargin{:});
    end
    function varargout = getSpecularColor(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(1655, self, varargin{:});
    end
    function varargout = setDiffuseColor(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(1656, self, varargin{:});
    end
    function varargout = getDiffuseColor(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(1657, self, varargin{:});
    end
    function self = ILight(varargin)
      if nargin==1 && strcmp(class(varargin{1}),'SwigRef')
        if ~isnull(varargin{1})
          self.swigPtr = varargin{1}.swigPtr;
        end
      else
        error('No matching constructor');
      end
    end
  end
  methods(Static)
  end
end
