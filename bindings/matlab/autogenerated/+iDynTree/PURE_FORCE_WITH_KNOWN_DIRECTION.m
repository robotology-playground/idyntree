function v = PURE_FORCE_WITH_KNOWN_DIRECTION()
  persistent vInitialized;
  if isempty(vInitialized)
    vInitialized = iDynTreeMEX(0, 10);
  end
  v = vInitialized;
end
