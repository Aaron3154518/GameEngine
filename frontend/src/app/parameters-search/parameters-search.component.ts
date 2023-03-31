import { Component, Input, OnInit } from '@angular/core';
import { ParameterService } from '../services/parameter.service';
import { UUID, searchScore, sortListAscending } from '../utils/utils';
import { ParameterGroup, Parameters } from '../utils/interfaces';
import {
  ColComponent,
  ColHeaderComponent,
  Column,
} from '../search/search.component';
import { Pipe, PipeTransform } from '@angular/core';

@Pipe({
  name: 'parametergroup',
  pure: false,
})
export class ParameterGroupPipe implements PipeTransform {
  transform(
    items: Set<UUID>,
    parameterService: ParameterService
  ): ParameterGroup[] {
    return [...items]
      .map((id: UUID) => parameterService.getParamGroup(id))
      .filter((g: ParameterGroup) => g.params.size > 0);
  }
}

@Component({
  selector: 'app-group',
  template: `
    <ng-container
      *ngFor="
        let group of data | parametergroup : parameterService;
        let first = first
      "
      ><span *ngIf="!first" class="vertical-line h-100"></span
      ><span
        class="hover"
        [ngClass]="[first ? 'ms-2 me-1' : 'mx-1']"
        [title]="group.name"
        ><app-var [data]="group.params"></app-var></span
    ></ng-container>
  `,
  styles: [
    `
      .hover:hover {
        background-color: rgba(200, 200, 200, 0.25);
      }
    `,
  ],
})
export class GroupComponent implements ColComponent {
  @Input() data: Set<UUID> = new Set<UUID>();

  constructor(protected parameterService: ParameterService) {}
}

// TODO: delete type-modal
@Component({
  selector: 'app-parameters-search',
  templateUrl: './parameters-search.component.html',
  styleUrls: ['./parameters-search.component.css'],
})
export class ParametersSearchComponent implements OnInit {
  _parameters: Parameters[] = [];
  _search: string = '';

  cols: Column[] = [
    { key: 'type', component: ColHeaderComponent },
    {
      key: 'groups',
      component: GroupComponent,
    },
  ];

  constructor(protected parameterService: ParameterService) {}

  get parameters(): Parameters[] {
    return this._parameters;
  }

  set parameters(list: Parameters[]) {
    this._parameters = [...list];
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
    this.parameterService.$paramSetsChanged.subscribe(
      () => (this.parameters = [...this.parameterService.paramSets])
    );
  }

  onSearchChanged(event: Event) {
    this.search = (event.target as HTMLInputElement).value;
  }

  sortParamLists() {
    sortListAscending(
      this.parameters,
      searchScore(this.search, (set: Parameters) => [set.name, set.type])
    );
  }

  listExists(name: string) {
    return this.parameters.findIndex((l: Parameters) => l.name === name) !== -1;
  }

  newSet(type: string) {
    this.parameterService.newParamSet(
      new Parameters({
        type: type,
      })
    );
  }
}
