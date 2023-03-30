import {
  AfterViewInit,
  Component,
  ElementRef,
  OnInit,
  ViewChild,
} from '@angular/core';
import { ParameterService } from '../services/parameter.service';
import { ParameterGroup } from '../utils/interfaces';
import { searchScore, sortListAscending } from '../utils/utils';

@Component({
  selector: 'app-enum-search',
  templateUrl: './enum-search.component.html',
  styleUrls: ['./enum-search.component.css'],
})
export class ParameterGroupSearchComponent implements OnInit, AfterViewInit {
  _groups: ParameterGroup[] = [];
  _search: string = '';
  newList: string = '';
  newVar: string = '';
  currGroup?: ParameterGroup;

  @ViewChild('listDummy', { static: true })
  listDummy?: ElementRef<HTMLSpanElement>;
  @ViewChild('varDummy', { static: true })
  varDummy?: ElementRef<HTMLSpanElement>;

  constructor(private parameterService: ParameterService) {}

  get groups(): ParameterGroup[] {
    return this._groups;
  }

  set groups(list: ParameterGroup[]) {
    this._groups = [...list];
    this.sortParamLists();
  }

  get search(): string {
    return this._search;
  }

  set search(text: string) {
    this._search = text;
    this.sortParamLists();
  }

  ngOnInit(): void {
    this.parameterService.$paramGroupsChanged.subscribe(
      () => (this.groups = [...this.parameterService.paramGroups])
    );
  }

  ngAfterViewInit(): void {
    this.sizeInput(
      document.getElementById('dumb') as HTMLInputElement,
      this.listDummy
    );
    this.groups.forEach((g: ParameterGroup) =>
      this.sizeInput(
        document.getElementById(g.name) as HTMLInputElement,
        this.varDummy
      )
    );
  }

  onSearchChanged(event: Event) {
    this.search = (event.target as HTMLInputElement).value;
  }

  createParameterGroup() {}

  sortParamLists() {
    sortListAscending(
      this.groups,
      searchScore(this.search, (pg: ParameterGroup) => [pg.name, ...pg.params])
    );
  }

  sizeInput(input: HTMLInputElement, dummy?: ElementRef<HTMLSpanElement>) {
    if (dummy) {
      let txt: string = input.value ? input.value : input.placeholder;
      dummy.nativeElement.innerHTML = txt.replaceAll(' ', '&nbsp;');
      input.style.width = `${dummy.nativeElement.offsetWidth}px`;
    }
  }

  onInput(event: Event) {
    let input: HTMLInputElement = event.target as HTMLInputElement;
    this.newList = input.value;
    this.sizeInput(input, this.listDummy);
  }

  onKeyPress(event: KeyboardEvent) {
    if (event.key === 'Enter' && this.newList) {
      this.parameterService.newParamGroup(
        new ParameterGroup({
          name: this.newList,
        })
      );
      this.newList = '';
    }
  }

  onVarInput(event: Event) {
    let input: HTMLInputElement = event.target as HTMLInputElement;
    this.newVar = input.value = `1${input.value}`
      .replace(RegExp('[^0-9a-zA-Z_]', 'g'), '')
      .replace(RegExp('[0-9]+'), '');
    this.sizeInput(input, this.varDummy);
  }

  onVarKeyPress(event: KeyboardEvent, group: ParameterGroup) {
    if (event.key === 'Enter' && this.newVar) {
      group.addParam(this.newVar);
      this.newVar = '';
    }
  }
}
