typedef union mutex mutex_t;

union mutex {
  unsigned u;
  struct {
    unsigned char locked;
    unsigned char contended;
  } b;
};

int mutex_init(mutex_t *m);

int mutex_destroy(mutex_t *m);

int mutex_lock(mutex_t *m);

int mutex_unlock(mutex_t*m);

int mutex_trylock(mutex_t*m);
