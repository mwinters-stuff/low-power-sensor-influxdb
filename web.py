Import("env")


def before_spiffs(source, target, env):
  print("XXXXXX before_spiffs")

env.AddPreAction("buildfs", before_spiffs)
print("XXXXX script")
