#ifndef _SETTINGS_H_
#define _SETTINGS_H_

enum SymlinkPolicy {
  FOLLOW_NONE = 0, // -P (default)
  FOLLOW_ARGS,     // -H
  FOLLOW_ALL       // -L
};

struct Settings {
  enum SymlinkPolicy symlinkPolicy;
};

#endif /* !_SETTINGS_H_ */
