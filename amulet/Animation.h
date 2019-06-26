
#ifndef AMULET_ANIMATION_H
#define AMULET_ANIMATION_H

class Animation
{
    public:
        virtual void init( int param1, int param2) =0;
        virtual void step( int frame) =0;
    protected:
        int _p1;
        int _p2;
};

#define ANIMATION_SUBCLASS_DECL(A) class A : public Animation {public: void init(int p1, int p2);void step(int step);};

#endif