import { Injectable } from '@angular/core';
import { BehaviorSubject, Observable } from 'rxjs';

export interface Enum {
  name: string;
  vals: string[];
}

@Injectable({
  providedIn: 'root',
})
export class EnumService {
  private enumList: Enum[] = [
    { name: 'Wizards', vals: ['Wizard', 'Crystal', 'Catalyst'] },
    { name: 'Generators', vals: [...Array(5).keys()].map((i) => `T${i}`) },
  ];
  private enumListsSubject: BehaviorSubject<Enum[]> = new BehaviorSubject<
    Enum[]
  >(this.enumList);
  $enumLists: Observable<Enum[]> = this.enumListsSubject.asObservable();

  constructor() {}

  newEnum(name: string, vals: string[] = []) {
    this.enumList.push({ name: name, vals: vals });
    this.enumListsSubject.next(this.enumList);
  }
}
