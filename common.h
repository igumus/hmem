#ifndef COMMON_H_
#define COMMON_H_

#define NOTIMPLEMENTED                                                         \
  do {                                                                         \
    fprintf(stderr, "%s:%d ERROR: not implemented: %s\n", __FILE__, __LINE__,  \
            __func__);                                                         \
  } while (0);

#define TODO(task) printf("%s:%s TODO: %s\n", __FILE__, __func__, (task))

#endif // COMMON_H_
