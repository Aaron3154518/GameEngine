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
  selector: 'app-parameter-group-search',
  templateUrl: './parameter-group-search.component.html',
  styleUrls: ['./parameter-group-search.component.css'],
})
export class ParameterGroupSearchComponent implements OnInit {
  _groups: ParameterGroup[] = [];
  _search: string = '';
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

  onSearchChanged(event: Event) {
    this.search = (event.target as HTMLInputElement).value;
  }

  sortParamLists() {
    sortListAscending(
      this.groups,
      searchScore(this.search, (pg: ParameterGroup) => [pg.name, ...pg.params])
    );
  }

  sizeInput(input: HTMLInputElement, dummy: HTMLSpanElement, val?: string) {
    if (val !== undefined) {
      input.value = val;
    }
    let txt: string = input.value ? input.value : input.placeholder;
    dummy.innerHTML = txt.replaceAll(' ', '&nbsp;');
    input.style.width = `${dummy.offsetWidth}px`;
  }

  onKeyPress(event: KeyboardEvent, input: HTMLInputElement) {
    if (event.key === 'Enter' && input.value) {
      this.parameterService.newParamGroup(
        new ParameterGroup({
          name: input.value,
        })
      );
      input.value = '';
    }
  }

  onVarInput(input: HTMLInputElement, dummy: HTMLSpanElement) {
    input.value = `1${input.value}`
      .replace(RegExp('[^0-9a-zA-Z_]', 'g'), '')
      .replace(RegExp('[0-9]+'), '');
    this.sizeInput(input, dummy);
  }

  onVarKeyPress(
    event: KeyboardEvent,

    input: HTMLInputElement,
    dummy: HTMLSpanElement,
    group: ParameterGroup
  ) {
    if (event.key === 'Enter' && input.value) {
      group.addParam(input.value);
      this.sizeInput(input, dummy, '');
    }
  }

  selectGroup(
    input: HTMLInputElement,
    dummy: HTMLSpanElement,
    group: ParameterGroup
  ) {
    this.currGroup = group;
    this.sizeInput(input, dummy);
  }
}