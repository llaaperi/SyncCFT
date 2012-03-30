//
//  Element.cc
//  SyncCFT
//
//  Created by Elo Matias on 30.3.2012.
//  Copyright (c) 2012 student. All rights reserved.
//

#include <iostream>

#include "Element.hh"

/*
 * Element copy constructor
 */
Element::Element(Element const& other) {
    setName(other.getName());
    setSize(other.getSize());
    setHash(other.getHash());
    setTimeStamp(other.getTimeStamp());
}

/*
 * Compare two elements
 * @param other The element being compared to
 * @return Returns 0 if elements are identical, +1 if this element is newer, or
 * -1 if the other element is newer or same age
 */
int Element::compare(Element const& other) const{
    if (getHash() == other.getHash())
        return 0;
    else {
        if (getTimeStamp() > other.getTimeStamp())
            return 1;
        else
            return -1;
    }       
}

