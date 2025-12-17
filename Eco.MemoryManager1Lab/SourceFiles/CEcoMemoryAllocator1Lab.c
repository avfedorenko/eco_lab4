/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   IEcoMemoryAllocator1
 * </сводка>
 *
 * <описание>
 *   Данный исходный код описывает реализацию интерфейсов IEcoMemoryAllocator1
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2016 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */

#include "IEcoSystem1.h"
#include "CEcoMemoryManager1Lab.h"


/*
 *
 * <сводка>
 *   Функция QueryInterface
 * </сводка>
 *
 * <описание>
 *   Функция QueryInterface для интерфейса IEcoMemoryAllocator1
 * </описание>
 *
 */
int16_t ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_QueryInterface(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv) {
    CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));

    if (me == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }

    /* Проверка и получение запрошенного интерфейса */
    if ( IsEqualUGUID(riid, &IID_IEcoMemoryManager1) ) {
        *ppv = &pCMe->m_pVTblIMgr;
        pCMe->m_pVTblIMgr->AddRef((IEcoMemoryManager1*)pCMe);
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoMemoryAllocator1) ) {
        *ppv = &pCMe->m_pVTblIMem;
        pCMe->m_pVTblIMgr->AddRef((IEcoMemoryManager1*)pCMe);
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoVirtualMemory1) ) {
        *ppv = &pCMe->m_pVTblIVirtual;
        pCMe->m_pVTblIMgr->AddRef((IEcoMemoryManager1*)pCMe);
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoUnknown) ) {
        *ppv = &pCMe->m_pVTblIMgr;
        pCMe->m_pVTblIMgr->AddRef((IEcoMemoryManager1*)pCMe);
    }
    else {
        *ppv = 0;
        return ERR_ECO_NOINTERFACE;
    }

    return ERR_ECO_SUCCESES;
}

/*
 *
 * <сводка>
 *   Функция AddRef
 * </сводка>
 *
 * <описание>
 *   Функция AddRef для интерфейса IEcoMemoryAllocator1
 * </описание>
 *
 */
uint32_t ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_AddRef(/* in */ IEcoMemoryAllocator1Ptr_t me) {
    CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));

    if (me == 0 ) {
        return -1;
    }

    return atomicincrement_int32_t(&pCMe->m_cRef);
}

/*
 *
 * <сводка>
 *   Функция Release
 * </сводка>
 *
 * <описание>
 *   Функция Release для интерфейса IEcoMemoryAllocator1
 * </описание>
 *
 */
uint32_t ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Release(/* in */ IEcoMemoryAllocator1Ptr_t me) {
    CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));

    if (me == 0 ) {
        return -1;
    }

    /* Уменьшение счетчика ссылок на компонент */
    atomicdecrement_int32_t(&pCMe->m_cRef);

    /* В случае обнуления счетчика, освобождение данных экземпляра */
    if ( pCMe->m_cRef == 0 ) {
        deleteCEcoMemoryManager1Lab_623E1838((IEcoMemoryManager1*)pCMe);
        return 0;
    }
    return pCMe->m_cRef;
}

/*
 *
 * <сводка>
 *   Функция Alloc
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
void* ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Alloc(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ uint32_t size) {
    CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));
    HeapBlock* pHead = 0;
    HeapBlock* pIt = 0;
    HeapBlock* pBest = 0;
    uint32_t bestRemainder = 0xFFFFFFFF;

    if (me == 0 || size == 0) {
        return 0;
    }

    if (pCMe->m_pHeapStart == 0) {
        return 0;
    }

    size = (size + (uint32_t)sizeof(voidptr_t) - 1u) & ~((uint32_t)sizeof(voidptr_t) - 1u);
    pHead = (HeapBlock*)pCMe->m_pHeapStart;

    for (pIt = pHead; pIt != 0; pIt = pIt->next) {
        if (pIt->freeFlag && pIt->dataSize >= size) {
            uint32_t remainder = pIt->dataSize - size;

            if (remainder < bestRemainder) {
                bestRemainder = remainder;
                pBest = pIt;

                if (bestRemainder == 0) {
                    break;
                }
            }
        }
    }

    if (pBest == 0) {
        return 0;
    }

    if (pBest->dataSize > size) {
        uint32_t tailPayload = pBest->dataSize - size;

        if (tailPayload >= (uint32_t)sizeof(HeapBlock) + (uint32_t)sizeof(voidptr_t)) {
            HeapBlock* pTail = (HeapBlock*)((char_t*)pBest + sizeof(HeapBlock) + size);

            pTail->dataSize = tailPayload - (uint32_t)sizeof(HeapBlock);
            pTail->freeFlag = 1;

            pTail->prev = pBest;
            pTail->next = pBest->next;

            if (pTail->next != 0) {
                pTail->next->prev = pTail;
            }

            pBest->next = pTail;
            pBest->dataSize = size;
        }
    }

    pBest->freeFlag = 0;

    pCMe->m_iAllocSize += pBest->dataSize;
    pCMe->m_iBlockNum  += 1;

    return (void*)((char_t*)pBest + sizeof(HeapBlock));
}

/*
 *
 * <сводка>
 *   Функция Free
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
void ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Free(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ voidptr_t pv) {
    CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));
    HeapBlock* block = 0;

    if (me == 0 || pv == 0) {
        return;
    }

    if (pCMe->m_pHeapStart == 0 || pCMe->m_pHeapEnd == 0) {
        return;
    }

    if ((char_t*)pv < (pCMe->m_pHeapStart + (int32_t)sizeof(HeapBlock)) || (char_t*)pv >= pCMe->m_pHeapEnd) {
        return;
    }

    block = (HeapBlock*)((char_t*)pv - sizeof(HeapBlock));
    if ((char_t*)block < pCMe->m_pHeapStart || (char_t*)block >= pCMe->m_pHeapEnd) {
        return;
    }

    if (block->freeFlag) {
        return;
    }

    block->freeFlag = 1;

    if (pCMe->m_iAllocSize >= block->dataSize) {
        pCMe->m_iAllocSize -= block->dataSize;
    } else {
        pCMe->m_iAllocSize = 0;
    }
    if (pCMe->m_iBlockNum > 0) {
        pCMe->m_iBlockNum -= 1;
    }

    while (block->next != 0 && block->next->freeFlag) {
        HeapBlock* next = block->next;
        block->dataSize += (uint32_t)sizeof(HeapBlock) + next->dataSize;
        block->next = next->next;
        if (block->next != 0) {
            block->next->prev = block;
        }
    }

    if (block->prev != 0 && block->prev->freeFlag) {
        HeapBlock* prev = block->prev;
        prev->dataSize += (uint32_t)sizeof(HeapBlock) + block->dataSize;
        prev->next = block->next;
        if (prev->next != 0) {
            prev->next->prev = prev;
        }
        block = prev;

        while (block->next != 0 && block->next->freeFlag) {
            HeapBlock* next = block->next;

            block->dataSize += (uint32_t)sizeof(HeapBlock) + next->dataSize;
            block->next = next->next;

            if (block->next != 0) {
                block->next->prev = block;
            }
        }
    }
}

/*
 *
 * <сводка>
 *   Функция Copy
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
void* ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Copy(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ voidptr_t pvDst, /* in */ voidptr_t pvSrc, /* in */ uint32_t size) {
    uint8_t* d = 0;
    const uint8_t* s = 0;
    uint32_t i = 0;

    if (pvDst == 0 || pvSrc == 0) {
        return pvDst;
    }

    if (size == 0 || pvDst == pvSrc) {
        return pvDst;
    }

    d = (uint8_t*)pvDst;
    s = (const uint8_t*)pvSrc;

    if (d < s || d >= (s + size)) {
        for (i = 0; i < size; ++i) {
            d[i] = s[i];
        }
    } else {
        for (i = size; i > 0; --i) {
            d[i - 1] = s[i - 1];
        }
    }

    return pvDst;
}

/*
 *
 * <сводка>
 *   Функция Realloc
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
void* ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Realloc(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ voidptr_t pv, /* in */ uint32_t size) {
    CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));
    HeapBlock* hdr = 0;
    uint32_t oldSize = 0;
    void* newPtr = 0;

    if (me == 0) {
        return 0;
    }

    if (pv == 0) {
        return CEcoMemoryAllocator1Lab_623E1838_Alloc(me, size);
    }

    if (size == 0) {
        CEcoMemoryAllocator1Lab_623E1838_Free(me, pv);
        return 0;
    }

    size = (size + (uint32_t)sizeof(voidptr_t) - 1u) & ~((uint32_t)sizeof(voidptr_t) - 1u);

    hdr = (HeapBlock*)((char_t*)pv - sizeof(HeapBlock));
    oldSize = hdr->dataSize;

    if (size <= oldSize) {
        uint32_t diff = oldSize - size;

        if (diff >= (uint32_t)sizeof(HeapBlock) + (uint32_t)sizeof(voidptr_t)) {
            HeapBlock* tail = (HeapBlock*)((char_t*)hdr + sizeof(HeapBlock) + size);

            tail->dataSize = diff - (uint32_t)sizeof(HeapBlock);
            tail->freeFlag = 1;

            tail->prev = hdr;
            tail->next = hdr->next;

            if (tail->next != 0) {
                tail->next->prev = tail;
            }

            hdr->next = tail;
            hdr->dataSize = size;

            if (pCMe->m_iAllocSize >= diff) {
                pCMe->m_iAllocSize -= diff;
            } else {
                pCMe->m_iAllocSize = 0;
            }

            CEcoMemoryAllocator1Lab_623E1838_Free(me, (void*)((char_t*)tail + sizeof(HeapBlock)));
            hdr->freeFlag = 0;
        }

        return pv;
    }

    if (hdr->next != 0 && hdr->next->freeFlag) {
        HeapBlock* next = hdr->next;
        uint32_t merged = hdr->dataSize + (uint32_t)sizeof(HeapBlock) + next->dataSize;

        if (merged >= size) {
            uint32_t extra = size - hdr->dataSize;
            uint32_t rest = merged - size;

            if (rest >= (uint32_t)sizeof(HeapBlock) + (uint32_t)sizeof(voidptr_t)) {
                HeapBlock* tail = (HeapBlock*)((char_t*)hdr + sizeof(HeapBlock) + size);

                tail->dataSize = rest - (uint32_t)sizeof(HeapBlock);
                tail->freeFlag = 1;

                tail->prev = hdr;
                tail->next = next->next;

                if (tail->next != 0) {
                    tail->next->prev = tail;
                }

                hdr->next = tail;
                hdr->dataSize = size;
            } else {
                hdr->next = next->next;
                if (hdr->next != 0) {
                    hdr->next->prev = hdr;
                }
                hdr->dataSize = merged;
                extra = hdr->dataSize - oldSize;
            }

            pCMe->m_iAllocSize += extra;
            return pv;
        }
    }

    newPtr = CEcoMemoryAllocator1Lab_623E1838_Alloc(me, size);
    if (newPtr == 0) {
        return 0;
    }

    CEcoMemoryAllocator1Lab_623E1838_Copy(me, newPtr, pv, oldSize);
    CEcoMemoryAllocator1Lab_623E1838_Free(me, pv);

    return newPtr;
}

/*
 *
 * <сводка>
 *   Функция Fill
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
void* ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Fill(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ voidptr_t pv, /* in */ char_t c, /* in */ uint32_t size) {
    uint8_t* p = 0;
    uint32_t i = 0;

    if (pv == 0) {
        return pv;
    }
    if (size == 0) {
        return pv;
    }
    p = (uint8_t*)pv;

    for (i = 0; i < size; ++i) {
        p[i] = (uint8_t)c;
    }

    return pv;
}

/*
 *
 * <сводка>
 *   Функция Compare
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
int16_t ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Compare(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ voidptr_t pv1, /* in */ voidptr_t pv2, /* in */ uint32_t size) {
    const uint8_t* a = 0;
    const uint8_t* b = 0;
    uint32_t i = 0;

    if (size == 0) {
        return 0;
    }

    if (pv1 == pv2) {
        return 0;
    }

    if (pv1 == 0) {
        return -1;
    }

    if (pv2 == 0) {
        return 1;
    }

    a = (const uint8_t*)pv1;
    b = (const uint8_t*)pv2;

    for (i = 0; i < size; ++i) {
        if (a[i] != b[i]) {
            return (a[i] < b[i]) ? (int16_t)-1 : (int16_t)1;
        }
    }

    return 0;
}
