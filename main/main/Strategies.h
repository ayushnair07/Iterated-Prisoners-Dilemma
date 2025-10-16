#pragma once
// Strategies.h
#pragma once
#include "Strategy.h"

// Standard Strategies
class ALLC : public Strategy { /* ... */ };
class ALLD : public Strategy { /* ... */ };
class TFT : public Strategy { /* ... */ };
class GRIM : public Strategy { /* ... */ };
class PAVLOV : public Strategy { /* ... */ };
class RND : public Strategy { /* ... */ };
class CTFT : public Strategy { /* ... */ };
class PROBER : public Strategy { /* ... */ };

// Two Original Strategies
class SuspiciousTFT : public Strategy { /* ... */ };
class ALTERNATE : public Strategy { /* ... */ };