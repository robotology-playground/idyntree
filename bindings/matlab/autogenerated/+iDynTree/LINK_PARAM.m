function v = LINK_PARAM()
  persistent vInitialized;
  if isempty(vInitialized)
    vInitialized = iDynTreeMEX(0, 26);
  end
  v = vInitialized;
end
