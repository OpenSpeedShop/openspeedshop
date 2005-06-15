//////////////////////////////// -*- C++ -*- //////////////////////////////
//
// AUTHOR
//    Peter Shirley, Cornell University, shirley@graphics.cornell.edu 
//
// COPYRIGHT
//    Copyright (c) 1995  Peter Shirley. All rights reserved.
//
//    Permission to use, copy, modify, and distribute this software for any
//    purpose without fee is hereby granted, provided that this entire
//    notice is included in all copies of any software which is or includes
//    a copy or modification of this software and in all copies of the
//    a copy or modification of this software and in all copies of the
//    supporting documentation for such software.
//
// DISCLAIMER
//    Neither the Peter Shirley nor Cornell nor any of their
//    employees, makes any warranty, express or implied, or assumes any
//    liability or responsibility for the accuracy, completeness, or
//    usefulness of any information, apparatus, product, or process
//    disclosed, or represents that its use would not infringe
//    privately-owned rights.  
//
///////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////
////////////////                  ///////////////// 
////////////////     ggBST.c      /////////////////
////////////////                  ///////////////// 
///////////////////////////////////////////////////


/////////////////////////////////
///////      ggBST      ////////
///////   definitions   //////// 
////////////////////////////////

template <class T>
ggBST<T>::ggBST() 
{
  root = 0;
  numItems = 0;
}

template <class T>
ggBST<T>::~ggBST()
{
  DestroySubtree(root);
}

template <class T>
ggBoolean ggBST<T>::InsertIntoSubtree(const ggString& Key, 
				      T* dataPtr,
				      ggBSTNode<T>* &nodePtr)
{
  if (nodePtr) {
    ggString currentKey = nodePtr->getKey();
    if (Key == currentKey) {
      nodePtr->SetData(*dataPtr);
      return ggTrue;
    }
    if (Key < currentKey) 
      return InsertIntoSubtree(Key, dataPtr, nodePtr->left);
// implied else
      return InsertIntoSubtree(Key, dataPtr, nodePtr->right);  
  }
  nodePtr = new ggBSTNode<T>(Key,*dataPtr);
  numItems++;
  return ggTrue;
}

template <class T>
ggBoolean ggBST<T>::Delete(const ggString& Key)
{
  ggBSTNode<T>** p = &root;
  ggBSTNode<T>** q = 0;
  ggBSTNode<T>* nodeP = 0;
  ggBSTNode<T>* nodeQ = 0;
  ggString currentKey = "";
  
  while ( ((nodeP = *p) != 0) && 
	  ((currentKey = nodeP->getKey()) != Key) ) {
    if (Key < currentKey) p = &(nodeP->left);
    else p = &(nodeP->right);
  }

  if (nodeP) {
    if (nodeP->right == 0) {
        *p = nodeP->left;
        delete nodeP;
    }
    else if (nodeP->left == 0) {
         *p = nodeP->right;
        delete nodeP;
    }
    else { // nodeP has both children
      if (nodeP->right->left == 0) { // right is inorder successor
          nodeP->right->left = nodeP->left;
          *p = nodeP->right;
          delete nodeP;
      }
      else {
         q = &(nodeP->right->left);
         while ( (nodeQ = *q)->left != 0) q = &(nodeQ->left); 
                // q now points to pointer to inorder successor of nodeP

         *q = nodeQ->right;
         *p = nodeQ;
         (*p)->left = nodeP->left;  // look right here
         (*p)->right = nodeP->right;

         delete nodeP;
      }
    }
    numItems--;
    return ggTrue;
    }
  return ggFalse;
}

template <class T>
T* ggBST<T>::find(const ggString& Key)
{
  ggBSTNode<T>* node = root;
  ggString currentKey;
  while(node) {
    currentKey = node->getKey();    
    if (Key == currentKey) return node->getData();
    if (Key > currentKey) 
      node = node->right;
    else 
      node = node->left;
  }
  return 0;
}

template <class T>
void ggBST<T>::fillTrain(ggTrain<T*>& train ) const {
  ggBSTNode<T>* node = root;
  if (node) node->fillTrain(train);
}
      
template <class T>
void ggBST<T>::DestroySubtree(ggBSTNode<T>* Root)
{
  if (Root) {
	  DestroySubtree(Root->left);
	  DestroySubtree(Root->right);
      delete Root;
  }
}

/////////////////////////////////
///////    ggBSTNode     ////////
///////   definitions   //////// 
/////////////////////////////////

template <class T>
void ggBSTNode<T>::fillTrain(ggTrain<T*>& train ) const
{
  if (left) left->fillTrain(train);
  train.Append(data);
  if (right) right->fillTrain(train);
}


template <class T>
ggBSTNode<T>::ggBSTNode(const ggString& Key, T& Data)
{
  left = right = 0;
  key = Key;
  data = &Data;    // Is this right?????????????
}
