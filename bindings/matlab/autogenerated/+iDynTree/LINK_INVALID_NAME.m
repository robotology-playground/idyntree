function varargout = LINK_INVALID_NAME(varargin)
  narginchk(0,1)
  if nargin==0
    nargoutchk(0,1)
    varargout{1} = iDynTreeMEX(778);
  else
    nargoutchk(0,0)
    iDynTreeMEX(779,varargin{1});
  end
end
